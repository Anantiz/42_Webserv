#include "cluster.hpp"

/*
   struct pollfd {
               int   fd;       --> file descriptor
               short events;   --> requested events
               short revents;  --> returned events
           };


   int poll(struct pollfd *fds, 	--> array of file descriptors(Sockets in our case)
   			nfds_t nfds,			--> number of file descriptors
			int timeout				--> timeout in milliseconds, set to 0 for non-blocking
			);

	struct sockaddr_in {
	           sa_family_t     sin_family;     AF_INET
	           in_port_t       sin_port;       Port number
	           struct in_addr  sin_addr;       IPv4 address
	       };

*/

void	Cluster::init_sockets()
{
	int size = _ports.size();
	for (int i=0; i<size; i++)
	{
		// Open an IPv4 socket for TCP connections
		int sfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sfd == -1)
			throw std::runtime_error("socket");

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;         // IPv4
		addr.sin_port = htons(_ports[i]);   // Port
		addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces

		int yes = 1;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(yes)) == -1)
			throw std::runtime_error("setsockopt: " + std::string(::strerror(errno)));
		if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
			throw std::runtime_error("bind: " + std::string(::strerror(errno)));
		if (listen(sfd, 10) == -1)
			throw std::runtime_error("listen: " + std::string(::strerror(errno)));
		_listen_socket_fds.push_back(sfd);
	}
}

int	Cluster::start()
{
	try {
		init_sockets();
	} catch (const std::exception &e) {
		std::cerr << "Error initializing socket: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	int size = _listen_socket_fds.size();
	for (size_t i=0; i<size; i++)
		_poll_fds.push_back((pollfd){_listen_socket_fds[i], POLLIN, 0});

	_to_remove.reserve(_max_events / 2);
	return run();
}