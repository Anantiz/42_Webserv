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

// struct requestKv {
//     std::string key;
//     std::string value;
// };

class Client {
public:

	enum ParserState {
		PARSING_HEADERS,
		PARSING_CONTENT,
		LOOKING_FOR_BOUNDARY
	};

	enum ClientStatus {
		IDLE,
		GETTING_HEADER,
		HEADER_ALL_RECEIVED,
		GETTING_BODY,
		BODY_ALL_RECEIVED,
		RESPONSE_READY,
		SENDING_RESPONSE,
		RESPONSE_SENT,
		KEEP_ALIVE,
		TO_CLOSE,
	};

	enum TreatmentStatus
	{
		NOTHING,
		AWAITING_HEADERS,
		TREATING_HEADERS,
		AWAITING_BODY,
		TREATING_BODY,
		DOWNLOADING_FILE,
		DONE,
	};

	Client(int fd);
	~Client();

	//response function


	void	error_response( const std::string& custom_page );

	void				getMethod( void );
	void				postMethod( void );
	void				deleteMethod( void );
	pollfd				&getPollfd();
	void				parseChunk();
	void				findBoundary( std::string &line );
	void				parseHeaders( std::string &line, std::map<std::string, std::string> &headers );
	void				parseContent( std::string &line );
	void				boundaryParser();
	bool				isLine();

	bool				parse_request();
	void				send_response();
	bool				parseFirstLine( std::string &line );
	bool				checkline( std::string &line, std::map<std::string, \
	 					std::string>	&intermheader );

public:
	// It's all public because we use this more as a struct than a class
	uint								access_port;
	sockaddr_in							client_addr;
	socklen_t							client_len;
	pollfd								poll_fd;

	Server*								server;
	ParserState							state;

	Http::Request						request;
	Http::Response						response;
	enum ClientStatus					connection_status; // Status of the whole connection
	enum TreatmentStatus				treatment_status;  // Specific stages of treatment
	bool								to_close;         // Close the conection after sending the whole
};

#endif // CLIENT_EVENT_HPP
