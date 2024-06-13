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

	// Meta [constructor, destructor, operators]
	server();
	~server();

private:

	// Network parameters
	const std::string _serverName;
	const std::vector<int> _ports;

	const bool _ssl_enforced;
	const std::string _ssl_certificate;
	const std::string _ssl_certificate_key;

	// Server parameters
	const std::vector<std::string> _root; // The global server root is the first one
		/*
			To manage DELETE
				> Any sub-routes will append their root here
				> If the DELETE path is not in the root, then it is forbidden
				> Kinda usefull to not have an `rm -rf /` in the config file  ( ༎ຶ ۝ ༎ຶ )
		*/

	const std::vector<std::string> _indexes; // Index.html, index.php ... So a vector of strings

	const std::vector<route> _routes;

	// Clients conections
	clientManager _clientManager;

	const logs _log;

public:

	// Methods

	/*
		** This ones interact with the `clientManager`
	*/
	void	acceptConnection();
	void	closeConnection();
	void	readConnection();
	void	writeConnection();
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

	// In our case, `route` is the same as nginx `location` ...
	route `expression` {}
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