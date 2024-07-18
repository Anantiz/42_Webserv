#include "cluster.hpp"

Cluster::Cluster(const char *config_file_path) {
	/// Parse the config file
	// [...]
}

Cluster::~Cluster() {

	for (int i=0; i<_listen_socket_fds.size(); i++)
		close(_listen_socket_fds[i]);
}