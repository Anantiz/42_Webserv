#include <vector>
#include <string>
#include <map>

#include <sys/poll.h>

// Potential classes that we will need

class Cluster;
class Server;
class Location;
class ServerClientsManager; // Handles events for a server

// These two might not be needed thanks to the struct client_event
class HttpRequest;
class HttpResponse;

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

/// @brief This bad boy will be used to store the request and response, it could even hold a mutex if we were allowed
struct client_event {
	uint				access_port;
	pollfd				*pollfd;

	/// The Request Header will be Read before being sent to the server, can't match the server otherwise
	enum e_method		method;
	enum e_protocol		protocol;
	std::string			uri;
	std::string			host;
	char*				body;
	size_t				body_size;

	Server*				server;
	uint				error;

	enum e_conection_status	connection_status;
};

class Cluster {
private:

	/** @brief
	*  It is a list of ports, each port has a list of names
	*    these names are associated with a server, hardcore type but straightforward
	*  e.i:
    *   [(80, [("server1.com", ptr_to_serv), ("server2.com", ptr_to_serv)]),
	*   (443, [("server1.com", ptr_to_serv), ("server2.com", ptr_to_serv)])]
	*/
	std::vector< std::pair< u_int16_t, std::vector< std::pair<std::string, Server*> > >>	servers_ports;


	// [...]


public:

	//[...]

	/**
	 * Server found
	 *   > host is set.
	 * Server not found
	 *   > error is set to 404
	 *   > server is set to NULL
	 */
	void	match_request(struct client_event &request) const
	{
		for (int i=0; i<servers_ports.size(); i++) {
			if (servers_ports[i].first == request.access_port)
			{
				for (int j=0; j<servers_ports[i].second.size(); j++) {
					if (servers_ports[i].second[j].first == request.host) {
						request.server = servers_ports[i].second[j].second;
						return ;
					}
				}
				// The used port has no server with the requested name
				request.error = 404;
				request.server = NULL;
				return ;
			}
		}
		// The used port is not in the list, weird case, but let's cover it
		request.error = 404;
		request.server = NULL;
	}
};


class Server {
private:
	std::vector<u_int16_t>			ports;
	std::vector<std::string>		names;
	std::vector<Location>			locations;

};
