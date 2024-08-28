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
	ConfigParser p(config_file_path);

	_servers = p.get_servers();
	if (_servers.size() == 0)
		throw std::runtime_error("No server found in the config file");
	_logger.devLog("Parsing Succesful");
	init_server_ports();
	_client_count = 0;
	_max_queue = 30;
	_max_clients = 300;
}

Cluster::~Cluster()
{
	_logger.infoLog("Shuting down the server");
	cleanup();
	_logger.infoLog("Server is down");
	exit(0);
}

void	Cluster::cleanup()
{
	// Close all listen sockets
	for (size_t i=0; i<_listen_socket_fds.size(); i++)
		close(_listen_socket_fds[i]);

	// Remove any remaining client
	for (client_pool_it it = _client_pool.begin(); it != _client_pool.end(); it++) {
		delete it->second;
	}

	// Remove all servers
	for (size_t i=0; i <_servers.size(); i++)
	{
		delete (Server *)(_servers[i]);
		_servers[i] = NULL;
	}
}

bool	*Cluster::get_run_ptr()
{
	return &_run;
}

void	Cluster::down()
{
	Cluster::_run = false;
};


void Cluster::init_server_ports()
{
	// #####
	// For all parsed servers, get all ports they are listening to
	// and add them to the _ports vector, they will be our listening sockets
	for (size_t i=0; i<_servers.size(); i++)
	{
		Server *s = _servers[i];

		std::vector<u_int16_t> p = s->get_ports();
		for (size_t j=0; j < p.size(); j++){
			if (!utils::in_ports(p[j], _ports))
				_ports.push_back(p[j]);
		}
	}
	//#### --End of _ports vector initialization

	// A pair Port-[Servers] where each server in this vector is itself a pair of server_name-server_ptr
	std::pair<u_int16_t, std::vector< std::pair<std::string, Server* > > > pair_port_vec_nptr;

	/**
	 * For each port in _ports, for Servers listening to that port
	 * Add a pair of server_name and server_ptr to the vector of servers
	 * associated with that port
	 *
	 * Complex to explain, but if you think about it, it's just the most
	 * straightforward way to do the job.
	 */
	for (size_t i=0; i<_ports.size(); i++)
	{
		uint16_t port = _ports[i];
		std::vector< std::pair<std::string, Server* > >	vec_name_ptr;
		pair_port_vec_nptr.first = port;              // Port
		pair_port_vec_nptr.second = vec_name_ptr;     // Vector[ServerName-ServerPtr]
		_servers_ports.push_back(pair_port_vec_nptr); // You can already push, the parser ensure there will
		                                              // be at least one server listening to the port

		/**
		 * For each server in _servers, if the server is listening to the port
		 * create a pair server_name-server_ptr, if the server_name is already in the
		 * vector, Log an error but continue without adding the server to the vector
		 * Otherwise, add the server to the vector
		 */
		for (size_t j=0; j<_servers.size(); j++)
		{
			Server *s = _servers[j];
			if (utils::in_ports(port, s->get_ports()))
			{
				/**
				 * Now, for each name the server has, check if it's already in the vector
				 *     > YES: Log an error and ignore it
				 *     > NO:  Add the server to the vector
				 */
				std::vector<std::string> names = s->get_names();
				for (size_t k=0; k<names.size(); k++)
				{
					std::string name = names[k];
					if (utils::in_servers(name, vec_name_ptr))
						_logger.errLog("Server name `" + name + "` already in the list, you might want to edit your config file, ignoring it for now");
					else
						vec_name_ptr.push_back(std::make_pair(name, s));
				}
			}
		}
	}
}
