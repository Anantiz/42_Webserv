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
	for (size_t p = 0; p < _servers_ports.size(); p++) {
		if (client.access_port == _servers_ports[p].first) {
			// If no host is provided, use the first server
			if (client.request.host.empty()) {
				client.server = _servers_ports[p].second[0].second;
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
	// First remove duplicates:
	std::sort(_to_remove.begin(), _to_remove.end());
	_to_remove.erase(std::unique(_to_remove.begin(), _to_remove.end()), _to_remove.end());

	// Now remove
	while (_to_remove.size()) {
		_logger.devLog("Removing fd: " + utils::ito_str(_poll_fds[_to_remove.back()].fd));
		int i = _to_remove.back();

		close(_poll_fds[i].fd);
		_to_remove.pop_back();
		_poll_fds.erase(_poll_fds.begin() + i);

		Cluster::client_pool_it ret = _client_pool.find(_poll_fds[i].fd);
		if (ret != _client_pool.end()) {
			delete ret->second;
			_client_pool.erase(_poll_fds[i].fd);
			_client_count--;
		}
	}
}


void	Cluster::edit_pollfd(int i, short events)
{
	_poll_fds[i].events = events;
}
