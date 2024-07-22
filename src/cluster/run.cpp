#include "cluster.hpp"

#include <unistd.h> // sleep

ClientEvent *Cluster::accept_client(int i)
{
	if (_poll_fds[i].revents & POLLIN) {
		_logs.devLog("New POLLIN Event on socket " + utils::ito_str(_poll_fds[i].fd));
		try {
			ClientEvent *new_client = new ClientEvent(_poll_fds[i].fd);
			_client_pool[new_client->getPollfd().fd] = new_client;
			_poll_fds.push_back(new_client->getPollfd());
			_events_count++;
		} catch (const std::exception &e) {
			_logs.debugLog("Error accepting connection: " + std::string(e.what()));
		}
	}
	ClientEvent::client_pool_it client = _client_pool.find(_poll_fds[i].fd);
	// Should never happen if we don't code like monkeys
	if (client == _client_pool.end())
		throw std::runtime_error("Client not found in pool, invalid socket, internal error");
	return client->second;
}

int	Cluster::run() {
	while (_run) {
		// Make this loop CPU-friendly for production
		#ifdef DEBUG_PROD
			sleep(1);
		#endif

		int events_count = poll(_poll_fds.data(), _poll_fds.size(), 0); // 0 for non-blocking
		if (!events_count)
			continue;
		if (events_count == -1)
			throw std::runtime_error("poll: " + std::string(::strerror(errno)));

		for (size_t i=0; i < _poll_fds.size(); ++i) {
			try {
				ClientEvent *client = accept_client(i);
				// The parser will save Error codes
				// they will be handled by send_response() if any,
				// no need to handle anything here
				client->parse_request();
				if (client->getRequest().connection_status == CEvent::CLOSED) {
					_to_remove.push_back(i);
				}
				else
					client->send_response();
			} catch (const std::exception &e) {
				_logs.debugLog("Error client conection: " + std::string(e.what()));
				continue;
			}
		}
		remove_clients(); // Remove after the loop to avoid iterator invalidation
	}
	return EXIT_SUCCESS;
}