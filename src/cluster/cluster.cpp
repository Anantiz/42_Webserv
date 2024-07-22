#include "cluster.hpp"

bool Cluster::_run = true;


Cluster::Cluster(const char *config_file_path) {
	/// Parse the config file
	// [...]

	/*
		Parser job:
			For each `Server` add it to the '_servers' vector --> To have all the servers in one place
			For each port, add it to the '_ports' vector --> To Open a listen socket
			For each ports found in a 'Server' Hash into '_servers_ports' and associate the server (name + ptr),
			  used to resolve client request
	*/
}

Cluster::~Cluster() {

	// Close all listen sockets
	for (size_t i=0; i<_listen_socket_fds.size(); i++)
		close(_listen_socket_fds[i]);

	// Remove any remaining client
	for (size_t i=0; i<_client_pool.size(); i++)
		delete _client_pool[i];
}

void	Cluster::down()
{
	Cluster::_run = false;
};
