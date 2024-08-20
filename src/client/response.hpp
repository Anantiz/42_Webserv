#pragma once

#include <string>
#include <map>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>

#include <sys/socket.h> // socket
#include <sys/poll.h>   // poll
#include <netinet/in.h> // sockaddr_in

#include "http/http.hpp"

class response
{
private:
	/* data */
public:
	void	getMethod( void );
	void	postMethod( void );
	void	deleteMethod( void );
	response(/* args */);
	~response();
};

response::response(/* args */)
{
}

response::~response()
{
}
