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

namespace CEvent {

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

	/**
	 * I don't know at all what to do with this
	 * most probaly useless feature
	 * we'll see
	 */
	enum e_conection_status {
		REQUEST,
		RESPONSE,
		CLOSED,
		KEEP_ALIVE
	};
};

/// @brief This bad boy will be used to store the request and response, it could even hold a mutex if we were allowed
struct s_client_event {
	uint								access_port;
	sockaddr_in							client_addr;
	socklen_t							client_len;
	pollfd								pollfd;

	/// The Request Header will be Read before being sent to the server, can't match the server otherwise
	enum CEvent::e_method				method;
	enum CEvent::e_protocol				protocol;
	std::string							uri;
	std::string							host;
	char*								body;
	size_t								body_size;
	Server*								server;
	uint								error;

	enum CEvent::e_conection_status	connection_status;
};

class ClientEvent {
private:

	struct s_client_event		_request;

public:

	typedef std::map<int, ClientEvent*>::iterator client_pool_it;
	
	ClientEvent(int fd);
	~ClientEvent();

	s_client_event		&getRequest();
	pollfd				&getPollfd();

	void				parse_request();
	void				send_response();
};



#endif // CLIENT_EVENT_HPP