#include "cluster.hpp"

#include <unistd.h> // sleep

Client *Cluster::accept_client(int i)
{
	Client *client = NULL;

	if (_poll_fds[i].revents & POLLIN) {
		_logger.devLog("New POLLIN Event on socket " + utils::ito_str(_poll_fds[i].fd));
		Client::client_pool_it client_it = _client_pool.find(_poll_fds[i].fd);

		if (client_it == _client_pool.end())
		{
			try {
				client = new Client(_poll_fds[i].fd);
				_logger.devLog("New client accepted with fd: " + utils::ito_str(client->getPollfd().fd));
				_client_pool[client->getPollfd().fd] = client;
				_poll_fds.push_back(client->getPollfd());
				_client_count++;
			} catch (const std::exception &e) {
				_logger.warnLog("Error accepting connection: " + std::string(e.what()));
			}
		}
		else
		{
			client = client_it->second;
			_logger.devLog("Client already in pool with fd: " + utils::ito_str(client->getPollfd().fd));
		}
	}
	return client;
}

#define DEBUG_PROD
int	Cluster::run() {
	while (_run) {
		// Make this loop CPU-friendly for production
		#ifdef DEBUG_PROD
			sleep(2);
		#endif

		int events_count = poll(_poll_fds.data(), _poll_fds.size(), 0); // 0 for non-blocking
		_logger.devLog("\n");
		_logger.devLog("Clients count: " + utils::ito_str(_client_count));
		_logger.devLog("Events count: " + utils::ito_str(events_count));
		if (!events_count)
			continue;
		if (events_count == -1)
			throw std::runtime_error("poll: " + std::string(::strerror(errno)));
		for (size_t i=0; i < _poll_fds.size(); ++i) {
			try {
				Client *client = accept_client(i);
				(void)client;
				// The parser will save Error codes
				// they will be handled by send_response() if any,
				// no need to handle anything here
				// client->parse_request();
				// if (client->connection_status == Http::CLOSED) {
				// 	_to_remove.push_back(i);
				// }
				// else
				// {
				// 	match_request_serv(*client);
				// 	client->send_response();
				// }
			} catch (const std::exception &e) {
				_logger.debugLog("Error client conection: " + std::string(e.what()));
				continue;
			}
		}
		remove_poll_fds(); // Remove after the loop to avoid iterator invalidation
	}
	return EXIT_SUCCESS;
}