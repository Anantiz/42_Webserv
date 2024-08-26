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

#include "prototypes.hpp"
#include "http/http.hpp"

struct requestKv {
    std::string key;
    std::string value;
};

class Client {
public:
	typedef std::map<int, Client*>::iterator client_pool_it;

	Client(int fd);
	~Client();

	//response function
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
	std::string							buffer;
	uint								access_port;
	sockaddr_in							client_addr;
	socklen_t							client_len;
	pollfd								poll_fd;
	std::map<std::string, std::string>  mainHeader;

	bool								multipart;
	
	Server*								server;
	ParserState							state;
	Http::Request						request;
	Http::Response						response;
	Http::Boundary						boundary;
	enum Http::e_conection_status		connection_status;
};



#endif // CLIENT_EVENT_HPP
