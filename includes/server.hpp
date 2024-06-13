#pragma once
// This file contains the general includes that are used in the project
// as well as pre-declarations of custom classes and functions
#include "webserv.hpp"

/*
	** Config file directives
		> Based on Nginx configuration file directives
*/
class server
{
public:

	server();
	~server();

private:

	const std::string _serverName;

	// Listen directive
	const std::vector<int> _ports;

	const bool _ssl_enforced;
	const std::string _ssl_certificate;
	const std::string _ssl_certificate_key;

	const std::vector<location> _locations;
};

/* Exac words in nginx


Make it so that you can recognize directives
allowed in a parent block

But if you are not in a block then they are not allowed

server
{
	listen		// Port number
	ssl_certificate
	ssl_certificate_key
	root		// default directory
	index		// default file in the directory

	// This is what the subjects refers as a `route`
	location `expression` {}
}
*/

/*
If multiples servers are defined
	> For same ports
		> Take the first one

*/


/*
Needs to handle:
	POST:
		> Clients Send data to the server
		> The server then does whatever it needs to do with the data
		> The server then sends a response back to the client
	GET:
		> Clients request data from the server
		> Server sends back the data or an error

*/