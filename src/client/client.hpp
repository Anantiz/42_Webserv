#pragma once
#ifndef CLIENT_EVENT_HPP
#define CLIENT_EVENT_HPP

#include <string>
#include <map>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>

#include <sys/socket.h> // socket
#include <sys/poll.h>   // poll
#include <netinet/in.h> // sockaddr_in

#include "prototypes.hpp"
#include "http/http.hpp"

class Client {
public:
	typedef std::map<int, Client*>::iterator client_pool_it;

	Client(int fd);
	~Client();

	pollfd				&getPollfd();

	void				parse_request();
	void				send_response();

public:
	// It's all public because we use this more as a struct than a class
	uint								access_port;
	sockaddr_in							client_addr;
	socklen_t							client_len;
	pollfd								poll_fd;


	// Set at least this one
	std::string							host;
	// Cluster::match_request_serv() will set this
	Server*								server;


	/// The Request Header will be Read before being sent to the server, can't match the server otherwise
	enum Http::e_method					method;
	enum Http::e_protocol				protocol;
	std::string							uri; // uris stands for Uniform Resource Identifier, it's the path of the requested file/dir
	char*								body;
	size_t								body_size;
	uint								error;

	enum Http::e_conection_status		connection_status;
};



#endif // CLIENT_EVENT_HPP