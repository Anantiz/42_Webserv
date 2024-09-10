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
	
	enum Boundarystatus {
		GETTING_HEADER_B,
		HEADER_ALL_RECEIVED_B,
		GETTING_BODY_B,
		BODY_ALL_RECEIVED_B
	};

	enum EndRequest {
		DONT,
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

	enum ResponseStatus {
		NONE,
		SENDING_HEADER,
		HEADER_SENT,
		SENDING_FROM_FILE,
		SENDING_FROM_BUFFER,
		DONE_SENDING,
	};

	Client( int arg_poll_fd, int );
	~Client();

	//response function
	//new clean parsing

	void				receive_request_data();
	//Handle header
	bool				get_header();
	bool				parse_header();
	bool				parseFirstLine();
	void				parsefirstheader();
	std::string			getFirstLine();
	bool				extract_headers( std::string &line );
	void				set_request_end_type();				
	bool				set_end_request( void );
	void				set_header_info();

	//Handle content
	bool				parse_content();
	void				parseChunk();
	void				parseBody();


	void				error_response( const std::string& custom_page );
	bool				end_contentlength( void );
	void				getMethod( void );
	void				postMethod( void );
	void				deleteMethod( void );
	pollfd				&getPollfd();
	void				findBoundary( std::string &line );
	void				parseHeaders( std::string &line, std::map<std::string, std::string> &headers );
	void				parseContent( std::string &line );
	void				boundaryParser();
	bool				isLine();
	bool				detect_end( void );
	bool				parse_request();
	void				send_response();
	bool				end_encodingchunk( void );
	bool				end_multipart( void );
	bool				end_connection( void );

	void    			finalize_response( void );

	void    			cleanup_for_next_request( void );

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
	char								buff[4096];
	std::vector<Http::Request>			request_boundary;
	Http::Request						request;
	Http::Response						response;
	enum EndRequest						eor;
	enum ClientStatus					connection_status;
	enum TreatingStatus					treating_status;
	enum ResponseStatus					response_status;
	bool								to_close; // Close the conection after sending the whole
};

#endif // CLIENT_EVENT_HPP
