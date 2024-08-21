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

#include "config_parser/parser.hpp"
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
	typedef std::vector< std::pair< u_int16_t, std::vector< std::pair<std::string, Server* > > > > evil_typedef_t;
	typedef std::map<int, Client*>::iterator client_pool_it;

	static bool						_run;

	// Config
	int								_max_queue;
	int								_max_clients;
	int								_client_count;
	logs							_logger;
	std::vector<u_int16_t>			_ports;
	std::vector<Server*>			_servers;
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
	int restart_attempt; // For sudden crashes, we try to restart the server at least 2 times

	bool		*get_run_ptr(); // For signal handling
	int			start();
	static void	down(); // Affects all instances

private:

	void	init_server_ports();
	void	init_sockets();
	int		run();
	void	cleanup();

	Client	*accept_or_create_client(int i);
	void	handle_pollin(int i, Client *client);
	void	handle_pollout(int i, Client *client);
	void	handle_anything_else(int i, Client *client);
	void	remove_closed_conections();

	void	build_error_response(Client *client);


	/**
	 * No "Host" header
	 *   > Do nothing (will get called again)
	 * Server is "" (empty)
	 *   > Server is set to the first server in the list
	 * Server is invalid (It was accessed by IP and `host` was set to gibberish)
	 *   > error is set to 404
	 *   > server is set to NULL
	 */
	void	match_request_serv(Client &request) const;
};

#endif // CLUSTER_HPP