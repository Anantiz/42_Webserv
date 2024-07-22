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

/// @brief This bad boy will be used to store the request and response, it could even hold a mutex if we were allowed
struct s_client_event {

};

class ClientEvent {
public:
	enum e_method {
		GET,
		POST,
		PUT,
		DELETE,
		HEAD,
		OPTIONS,
		TRACE,
		CONNECT,
		PATCH
	};

	enum e_protocol {
		HTTP_1_0,
		HTTP_1_1,
		HTTP_2_0,
		Others
	};

	enum e_conection_status {
	/**
	 * I don't know at all what to do with this
	 * most probaly useless feature
	 * we'll see
	 */
		REQUEST,
		RESPONSE,
		CLOSED,
		KEEP_ALIVE
	};

	typedef std::map<int, ClientEvent*>::iterator client_pool_it;

	ClientEvent(int fd);
	~ClientEvent();

	pollfd				&getPollfd();

	void				parse_request();
	void				send_response();

public:
	// It's all public because we use this more as a struct than a class
	uint								access_port;
	sockaddr_in							client_addr;
	socklen_t							client_len;
	pollfd								poll_fd;

	/// The Request Header will be Read before being sent to the server, can't match the server otherwise
	enum e_method						method;
	enum e_protocol						protocol;
	std::string							uri;
	std::string							host;
	char*								body;
	size_t								body_size;
	Server*								server;
	uint								error;

	enum e_conection_status				connection_status;
};



#endif // CLIENT_EVENT_HPP