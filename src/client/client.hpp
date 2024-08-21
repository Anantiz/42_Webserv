#pragma once
#ifndef CLIENT_EVENT_HPP
#define CLIENT_EVENT_HPP

#include <string>
#include <map>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <sys/socket.h> // socket
#include <sys/poll.h>   // poll
#include <netinet/in.h> // sockaddr_in

#include "http/http.hpp"

/*
struct pollfd {
            int   fd;       --> file descriptor
            short events;   --> requested events
            short revents;  --> returned events
        };
*/

class Server; // Forward declaration

struct requestKv {
    std::string key;
    std::string value;
};

class Client {
public:

	enum client_status {
		IDLE,
		GETTING_HEADER,
		HEADER_ALL_RECEIVED,
		GETTING_BODY,       // Can work together
		BODY_ALL_RECEIVED,
		TREATING_REQUEST,   // Can work together
		RESPONSE_READY,
		SENDING_RESPONSE,
		RESPONSE_SENT,
		KEEP_ALIVE,
		TO_CLOSE,
	};

	enum treting_status
	{
		awaiting_headers,
		awaiting_body,
		uploading,
		waiting_body_chunk,
	};

	Client(int fd);
	~Client();

	//response function
	void	getMethod( void );
	void	postMethod( void );
	void	deleteMethod( void );
	pollfd	&getPollfd();

	bool	parse_request();
	void	send_response();
	bool	parseFirstLine( std::string &line,
							std::string &methodstr,
							std::string &pathstr,
							std::string &protocolstr
						);

public:
	// It's all public because we use this more as a struct than a class

	uint								access_port;
	sockaddr_in							client_addr;
	socklen_t							client_len;
	pollfd								poll_fd;
	Server*								server;

	Http::Request						request;
	Http::Response						response;
	enum client_status					connection_status;
	bool								to_close; // Close the conection after sending the whole
};



#endif // CLIENT_EVENT_HPP