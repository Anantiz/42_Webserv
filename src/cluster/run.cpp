#include "cluster.hpp"

#include <unistd.h> // sleep

void	Cluster::handle_pollin(int i, Client *client)
{
	// char buff[4096] = {0};
	_logger.devLog("Pollin on fd:" + utils::ito_str(client->poll_fd.fd));
	client->parse_request();
	// ssize_t red = recv(client->poll_fd.fd, buff, 4096, 0);
	// _logger.devLog("Bytes read: " + utils::ito_str(red) + " content: " + std::string(buff));
	// client->connection_status = Client::TO_CLOSE;
	// Close invalid requests, or unexpected connection termination
	if (client->connection_status == Client::TO_CLOSE) {
		_logger.devLog("Error Killing conection: " + utils::ito_str(client->poll_fd.fd));
		_to_remove.push_back(i);
		return ;
	}

	if (client->server)
	{
		client->server->handle_client_request(*client);
	}
	else if (client->connection_status == Client::HEADER_ALL_RECEIVED \
		|| client->connection_status == Client::GETTING_BODY \
		|| client->connection_status == Client::BODY_ALL_RECEIVED)
	{
		match_request_serv(*client);
		if (!client->server)
		{
			_logger.devLog("Client didn't send `host` header, forbidden in Http/1.1, sending 400");
			client->response.status_code = 400;
			client->error_response("");
			// throw error
			return ; // for now
		}
		_logger.devLog("Client request matched with server");
		client->server->handle_client_request(*client);
	}
	else
	{
		_logger.devLog("Client request not ready to be matched with server");
	}
}

void	Cluster::handle_pollout(int i, Client *client)
{
	_logger.devLog("Pollout:" + utils::ito_str(client->poll_fd.fd));
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

void	Cluster::handle_anything_else(int i, Client *client)
{
	_logger.devLog("POLL other:" + utils::ito_str(client->poll_fd.fd));
	if (_poll_fds[i].revents & POLLHUP || _poll_fds[i].revents & POLLERR)
	{
		_logger.devLog("Client disconnected");
		client->connection_status = Client::TO_CLOSE;
		_to_remove.push_back(i);
	}
	return ;
}

Client *Cluster::accept_or_create_client(int i)
{
	Client *client = NULL;

	if (_poll_fds[i].revents == 0)
		return NULL;
	_logger.devLog("Event on socket with fd: " + utils::ito_str(_poll_fds[i].fd));
	client_pool_it client_it = _client_pool.find(_poll_fds[i].fd);

	if (client_it == _client_pool.end())
	{
		try {
			if (i > (int)_ports.size())
				throw std::runtime_error("Invalid socket, client not in pool and using an invalid socket");
			client = new Client(_poll_fds[i].fd, _ports[i]);
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
		_logger.devLog("Reusing client from pool with fd: " + utils::ito_str(client->getPollfd().fd));
	}
	return client;
}

#define DEBUG_PROD
int	Cluster::run()
{
	_logger.infoLog("Cluster started");
	int error = 0;

	while (_run) {
		// Make this loop CPU-friendly for production
		#ifdef DEBUG_PROD
			sleep(1);
		#endif

		printf("\n");
		_logger.devLog("_poll_fds length: " + utils::ito_str(_poll_fds.size()));
		int events_count = poll(_poll_fds.data(), _poll_fds.size(), 0);
		if (!events_count)
			continue;
		if (events_count == -1)
		{
			// Just try to recover if it was a temporary error
			if (error == 10)
				throw std::runtime_error("Unresolved Critical error: " + std::string(::strerror(errno)));
			error++;
			continue;
		}
		_logger.devLog("Clients count: " + utils::ito_str(_client_count));
		_logger.devLog("Events count: " + utils::ito_str(events_count));

		for (size_t i = 0; i < _poll_fds.size(); i++)
		{
			// std::cout << "NEW TURN OF GETTING DATA INTO BUFFER" << std::endl;
			Client *client = NULL;
			try {
				client = accept_or_create_client(i);
			} catch (const std::exception &e) {
				_logger.debugLog("Error client conection: " + std::string(e.what()));
				continue;
			}
			if (!client || i < _ports.size())
				continue;
			if (_poll_fds[i].revents & POLLIN)
				handle_pollin(i, client);
			if (_poll_fds[i].revents & POLLOUT)
				handle_pollout(i, client);
			if (_poll_fds[i].revents & ~(POLLIN | POLLOUT))
				handle_anything_else(i, client);
		}
		remove_closed_conections(); // Remove after the loop to avoid iterator invalidation
		error = 0; // Error where resolved if we reached this point
		this->restart_attempt = 0; // The server is running fine, reset the counter
	}
	cleanup();
	return EXIT_SUCCESS;
}
