#pragma once
#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include <vector>
#include <map>

#include <string>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>	 // exit
#include <cstdlib>
#include <string.h> // perror
#include <errno.h>

#include <sys/socket.h> // socket
#include <sys/poll.h>   // poll
#include <netinet/in.h> // sockaddr_in

#include "server/server.hpp"
#include "client_event/client_event.hpp"
#include "utils/logs.hpp"


class Cluster {
private:

	/** @brief
	*  It is a list of ports, each port has a list of names
	*    these names are associated with a server, hardcore type but straightforward
	*  e.i:
    *   [(80, [("server1.com", ptr_to_serv), ("server2.com", ptr_to_serv)]),
	*   (443, [("server1.com", ptr_to_serv), ("server2.com", ptr_to_serv)])]
	*/
	typedef \
		std::vector< std::pair< u_int16_t, std::vector< std::pair<std::string, Server*> > > > \
		ss_reichfuhrer_t; // ss_reichfuhrer_t cuz it's the worst eveil type possible

	static bool						_run;
	int								_max_queue = 10;
	int								_max_events = 50;
	int								_events_count = 0;
	logs							_logs;

	std::vector<u_int16_t>			_ports;
	std::vector<Server>				_servers;
	ss_reichfuhrer_t				_servers_ports;

	std::vector<int>				_listen_socket_fds;
	std::vector<struct pollfd>		_poll_fds;

	std::map<int, ClientEvent*>		_client_pool;
	std::vector<int>				_to_remove;


public:

	Cluster(const char *config_file_path);
	~Cluster();

	bool	*get_run_ptr(); // For signal handling

	int		Cluster::start();
	static void	Cluster::down() { _run = false; };

private:

	/**
	 * Server found
	 *   > host is set.
	 * Server not found
	 *   > error is set to 404
	 *   > server is set to NULL
	 */
	void	match_request_serv(struct s_client_event &request) const;

	void	Cluster::init_sockets();
	int		Cluster::run();

	ClientEvent		*Cluster::accept_client(int i);
	void			remove_clients();

};

#endif // CLUSTER_HPP
