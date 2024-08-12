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
	_logger.devLog("Parsing done");
	_servers = p.get_servers();
	_logger.devLog("Initiating the demon");
	init_server_ports();
}

void Cluster::init_server_ports()
{

	for (size_t i=0; i<_servers.size(); i++)
	{
		Server *s = _servers[i];

		std::vector<u_int16_t> p = s->get_ports();
		for (size_t j=0; j < p.size(); j++){
			if (!utils::in_ports(p[i], _ports))
				_ports.push_back(p[i]);
		}
	}
	std::pair<u_int16_t, std::vector< std::pair<std::string, Server* > > > port_servs_pair;

	for (size_t i=0; i<_ports.size(); i++)
	{
		uint16_t port = _ports[i];
		std::vector< std::pair<std::string, Server* > >\
			servs_per_port_vector;

		port_servs_pair.first = _ports[i];
		for (size_t j=0; j<_servers.size(); j++)
		{
			Server *s = _servers[j];
			if (utils::in_ports(port, s->get_ports()))
			{
				std::pair<std::string, Server* >\
					serv_name_ptr_pair;
				std::vector<std::string> names = s->get_names();
				for (size_t k=0; k<names.size(); k++)
				{
					serv_name_ptr_pair.first = names[k];
					serv_name_ptr_pair.second = s;
					servs_per_port_vector.push_back(serv_name_ptr_pair);
				}
			}
		}
		port_servs_pair.second = servs_per_port_vector;
		_servers_ports.push_back(port_servs_pair);
	}
}

Cluster::~Cluster() {

	// Close all listen sockets
	for (size_t i=0; i<_listen_socket_fds.size(); i++)
		close(_listen_socket_fds[i]);

	// Remove any remaining client
	for (size_t i=0; i<_client_pool.size(); i++)
		delete _client_pool[i];

	for (size_t i=0; i<_servers.size(); i++)
		delete _servers[i];
}

void	Cluster::down()
{
	Cluster::_run = false;
};
