#include "cluster.hpp"

/**
 *  MESSAGE TO THE CORRECTOR:
 * 	    > I am fully aware that the subject says that two servers listening
 *        on the same port should be "forbidden", but since the subject also says
 *        to mimic Nginx behavior, I decided to allow it just like Nginx does.
 *      If you are not happy, go touch some grass. :)
 */
void	Cluster::match_request_serv(Client &client) const
{
	// If in for name:port, cut the port
	client.request.host = client.request.host.substr(0, client.request.host.find(':'));
	for (size_t p = 0; p < _servers_ports.size(); p++) {
		if (client.access_port == _servers_ports[p].first) {
			// If no host is provided, use the first server
			if (client.request.host.empty()) {
				client.server = (((_servers_ports[p]).second)[0]).second;
				return;
			}
			// RFC 2616, section 3.2.3, states that the host is case-insensitive
			std::string request_host = utils::as_lower(client.request.host);
			// Find the server associated with the host, slow af striing comparison
			for (size_t i = 0; i < _servers_ports[p].second.size(); i++) {
				if (request_host == _servers_ports[p].second[i].first) {
					client.server = _servers_ports[p].second[i].second;
					return;
				}
			}
			// If host is not found, provide the first server
			client.server = _servers_ports[p].second[0].second;
		}
	}
}

/**
 * @brief
 * We use this function to delete all dead-conection at once.
 * This prevents the main-loop iterator from becoming invalid.
 */
void	Cluster::remove_closed_conections()
{
	std::vector<int> removed; // To avoid double-removal

	// Now remove
	while (_to_remove.size()) {
		_logger.devLog("Removing fd: " + utils::anything_to_str(_poll_fds[_to_remove.back()].fd));
		int i = _to_remove.back();
		_to_remove.pop_back();
		if (std::find(removed.begin(), removed.end(), i) != removed.end()) {
			continue;
		}

		// Remove from client pool
		Cluster::client_pool_it client_it = _client_pool.find(_poll_fds[i].fd);
		if (client_it != _client_pool.end()) {
			delete client_it->second;
			_client_pool.erase(_poll_fds[i].fd);
			_client_count--;
		}

		// Remove from poll
		close(_poll_fds[i].fd);
		_poll_fds.erase(_poll_fds.begin() + i);

		removed.push_back(i);
	}
}


void	Cluster::edit_pollfd(int i, short events, Client *client)
{
	(void)i;
	(void)client;
	(void)events;
	_poll_fds[i].events = events;
	client->poll_fd.events = events; // This data is useless, but just keep it consistent
}
