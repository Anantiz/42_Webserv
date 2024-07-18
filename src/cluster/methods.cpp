#include "cluster.hpp"

void	Cluster::match_request_serv(struct s_client_event &request) const
{
		for (int i=0; i<servers_ports.size(); i++) {
			if (servers_ports[i].first == request.access_port)
			{
				for (int j=0; j<servers_ports[i].second.size(); j++) {
					if (servers_ports[i].second[j].first == request.host) {
						request.server = servers_ports[i].second[j].second;
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
