#include "cluster.hpp"

/*
	As long as the Client 'host' is set, this function will set the 'server' pointer
*/
void	Cluster::match_request_serv(Client &request) const
{
		for (size_t i=0; i<_servers_ports.size(); i++) {
			if (_servers_ports[i].first == request.access_port)
			{
				for (size_t j=0; j<_servers_ports[i].second.size(); j++) {
					if (_servers_ports[i].second[j].first == request.host) {
						request.server = _servers_ports[i].second[j].second;
						return ;
					}
				}
				// The used port has no server with the requested name
				request.error = 404;
				request.server = NULL;
				return ;
			}
		}
		// The used port is not in the list, weird case, but let's cover it
		request.error = 404;
		request.server = NULL;
}

bool	*Cluster::get_run_ptr() { return &_run; }

void	Cluster::remove_poll_fds() {
	while (_to_remove.size()) {
		int i = _to_remove.back();
		_to_remove.pop_back();
		_client_pool.erase(_poll_fds[i].fd);
		_poll_fds.erase(_poll_fds.begin() + i);
	}
}