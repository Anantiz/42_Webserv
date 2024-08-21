#include "cluster.hpp"

#include <unistd.h> // sleep

Client *Cluster::accept_or_create_client(int i)
{
	Client *client = NULL;

	_logger.devLog("New Event on socket " + utils::ito_str(_poll_fds[i].fd));
	client_pool_it client_it = _client_pool.find(_poll_fds[i].fd);

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
	return client;
}

void	Cluster::handle_pollin(int i, Client *client)
{
	client->parse_request();

	// Close invalid requests, or unexpected connection termination
	if (client->connection_status == Client::TO_CLOSE) {
		_to_remove.push_back(i);
		return ;
	}
	//
	if (!client->server)
	{
		match_request_serv(*client);
		if (!client->server && client->connection_status == Client::HEADER_ALL_RECEIVED)
		{
			_logger.devLog("Client didn't send `host` header, forbidden in Http/1.1, sending 400");
			client->response.status_code = 400;
			build_error_response(client);
			client->connection_status = Client::SENDING_RESPONSE;
			return ;
		}
	}
	if (client->server)
		client->server->handle_client_request(*client);
}

void		Cluster::handle_pollout(int i, Client *client)
{
	if (client->connection_status == Client::SENDING_RESPONSE)
	{
		client->send_response();
		if (client->connection_status == Client::RESPONSE_SENT)
		{
			if (client->to_close)
			{
				client->connection_status = Client::TO_CLOSE;
				_to_remove.push_back(i);
			}
			else
				client->connection_status = Client::KEEP_ALIVE;
		}
	}

	// Technically, we should never reach this point, but just in case
	if (client->connection_status == Client::TO_CLOSE)
	{
		_logger.devLog("Run.cpp Handle_pollout: Closing connection, Shouldn't be here");
		_to_remove.push_back(i);
	}
	return ;
}

void		Cluster::handle_anything_else(int i, Client *client)
{
	if (_poll_fds[i].revents & POLLHUP || _poll_fds[i].revents & POLLERR)
	{
		_logger.devLog("Client disconnected");
		client->connection_status = Client::TO_CLOSE;
		_to_remove.push_back(i);
	}
	return ;
}


#define DEBUG_PROD
int	Cluster::run() {
	int error = 0;
	int ret = 0;
	while (_run) {
		// Make this loop CPU-friendly for production
		#ifdef DEBUG_PROD
			sleep(2);
		#endif

		int events_count = poll(_poll_fds.data(), _poll_fds.size(), 0); // 0 for non-blocking
		if (!events_count)
			continue;
		if (events_count == -1)
		{
			if (error == 5)
				throw std::runtime_error("Unresolved Critical error: " + std::string(::strerror(errno)));
			error++;
			continue;
		}
		_logger.devLog("\n");
		_logger.devLog("Clients count: " + utils::ito_str(_client_count));
		_logger.devLog("Events count: " + utils::ito_str(events_count));
		for (size_t i = 0; i < _poll_fds.size(); i++)
		{
			Client *client = NULL;
			try {
				client = accept_or_create_client(i);
			} catch (const std::exception &e) {
				_logger.debugLog("Error client conection: " + std::string(e.what()));
				return 1;
			}
			if (!client)
				continue;
			if (_poll_fds[i].revents & POLLIN)
				handle_pollin(i, client);
			if (_poll_fds[i].revents & POLLOUT)
				handle_pollout(i, client);
			else
				handle_anything_else(i, client);
		}
		remove_closed_conections(); // Remove after the loop to avoid iterator invalidation
		error = 0;
		this->restart_attempt = 0; // The server is running fine, reset the counter
	}
	cleanup();
	return EXIT_SUCCESS;
}
