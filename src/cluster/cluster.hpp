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
#include "client/client.hpp"
#include "http/http.hpp"
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
		evil_typedef_t; // evil_typedef cuz it's the worst evil type possible

	static bool						_run;

	// Config
	int								_max_queue;
	int								_max_events;
	int								_max_clients;
	int								_events_count;
	logs							_logger;
	std::vector<u_int16_t>			_ports;
	std::vector<Server>				_servers;
	evil_typedef_t					_servers_ports;

	// start()
	std::vector<int>				_listen_socket_fds;

	// run()
	std::vector<struct pollfd>		_poll_fds;
	std::map<int, Client*>			_client_pool;
	std::vector<int>				_to_remove;


public:

	Cluster(const char *config_file_path);
	~Cluster();

	bool	*get_run_ptr(); // For signal handling

	int			start();
	static void	down(); // Affects all instances

private:

	void	init_sockets();
	int		run();

	Client		*accept_client(int i);
	void			remove_poll_fds();

	/**
	 * Server found
	 *   > host is set.
	 * Server not found
	 *   > error is set to 404
	 *   > server is set to NULL
	 */
	void	Cluster::match_request_serv(Client &request) const;

};

#endif // CLUSTER_HPP
