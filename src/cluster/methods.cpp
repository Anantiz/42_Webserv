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
			// Find the server associated with the host, slow af striing comparison
			for (size_t i = 0; i < _servers_ports[p].second.size(); i++) {
				if (client.request.host == _servers_ports[p].second[i].first) {
					client.server = _servers_ports[p].second[i].second;
					return;
				}
			}
			// If host is not found, provide the first server
			client.server = _servers_ports[p].second[0].second;
		}
	}
}

bool	*Cluster::get_run_ptr() { return &_run; }

void	Cluster::remove_poll_fds() {
	while (_to_remove.size()) {
		int i = _to_remove.back();
		_to_remove.pop_back();
		_client_pool.erase(_poll_fds[i].fd);
		_poll_fds.erase(_poll_fds.begin() + i);
		_client_count--;
	}
}