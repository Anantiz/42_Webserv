// #pragma once
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
#include <cstring>  // for strlen

/*
struct pollfd {
            int   fd;       --> file descriptor
            short events;   --> requested events
            short revents;  --> returned events
        };
*/

bool	gnlEcoplus( std::string &str, std::string &result );


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
		BODY_ALL_RECEIVED_B,
		SEARCH_BOUNDARY_FIRST_LINE
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
		SENDING_PRE_FILLED_BODY,
		DONE_SENDING,
	};

	Client( int entry_socket_fd, int arg_access_port);
	~Client();

	//response function
	//new clean parsing
	void				print_request();
	void				receive_request_data();
	//Handle header
	bool				get_header();
	void				parse_header();
	void				parseFirstLine();
	void				parsefirstheader();
	std::string			getFirstLine();
	void				extract_headers( std::string &line );
	bool				set_end_request( void );
	void				clean_first_boundary_b();

	//Handle content
	void				parse_content();
	void				parseChunk();
	void				parseBody();
	//multipart content
	bool				get_header_b();
	void				parse_header_b();
	void				parsefirstheader_b();
	void				extract_headers_b( std::string &line );
	void				parse_content_b();

	void				error_response( const std::string& custom_page );
	bool				end_contentlength( void );
	pollfd				&getPollfd();
	void				boundaryParser();
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
	Http::Request						b_request;
	logs								_logger;
	int									access_port;
	sockaddr_in							client_addr;
	socklen_t							client_len;
	pollfd								poll_fd;
	Server*								server;
	ParserState							state;
	bool								isHeader;
	bool								isFirstLine;
	bool								isFirstLine_b;
	char								buff[4096];
	std::vector<Http::Request>			request_boundary;
	Http::Request						request;
	Http::Response						response;
	enum EndRequest						eor;
	enum ClientStatus					connection_status;
	enum Boundarystatus					b_connection_status;
	enum TreatingStatus					treating_status;
	enum ResponseStatus					response_status;
	bool								to_close; // Close the conection after sending the whole
};

#endif // CLIENT_EVENT_HPP
