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
#include <utils/logs.hpp>
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

	enum EndRequest {
		CONTENT_LENGTH,
		ENCODING_CHUNK,
		CONNECTION,
		MULTIPART,
	};

	enum TreatingStatus
	{
		NOTHING,
		AWAITING_HEADERS,
		TREATING_HEADERS,
		AWAITING_BODY,
		TREATING_BODY,
		DOWNLOADING_FILE,
		DONE,
	};

	Client( int arg_poll_fd, int );
	~Client();

	//response function


	void				error_response( const std::string& custom_page );
	bool				end_contentlength( void );
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
	bool				detect_end( void );
	bool				parse_request();
	void				send_response();
	bool				parseFirstLine( std::string &line );
	bool				checkline( std::string &line, std::map<std::string, \
	 					std::string>	&intermheader );
	bool				end_request( void );
	bool				end_encodingchunk( void );
	bool				end_multipart( void );
	bool				end_connection( void );


public:
	// It's all public because we use this more as a struct than a class
	logs								_logger;
	int									access_port;
	sockaddr_in							client_addr;
	socklen_t							client_len;
	pollfd								poll_fd;
	Server*								server;
	ParserState							state;
	bool								isHeader;
	bool								isFirstLine;

	Http::Request						request;
	Http::Response						response;
	enum EndRequest						eor;
	enum ClientStatus					connection_status;
	enum TreatingStatus					treating_status;
	bool								to_close; // Close the conection after sending the whole
};

#endif // CLIENT_EVENT_HPP
