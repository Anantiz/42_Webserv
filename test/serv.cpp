#include "serv.hpp"


void serv::start()
{
	std::cout << "Starting server " << _name << " on port " << _port << " with root " << _root << std::endl;

	// Create an endpoint for IO communication with whatever is on the other side of the socket
	//int socket(int domain, int type, int protocol);
	int	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) { std::cerr << "Error creating socket" << std::endl;	throw std::runtime_error("Socket");}

	// int setsockopt(int sockfd,
	// 			int level, --> SOL_SOCKET (Idk what the others are for, but this one let's you do anything)
	// 			int optname, --> SO_REUSEADDR | SO_REUSEPORT
	// 			const void optval[...], --> &opt with [int opt = 1] // Wtf is this for ? libc dudes are weird, just send a damn bool ?
	// 			socklen_t optlen); sizeof(opt)
	int yes = 1; // Nah, really, what is that for ?
	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(yes)) == -1)
	{ std::cerr << "Error setting socket options" << std::endl; throw std::runtime_error("setsockopt"); }

	// Required for bind()
	struct sockaddr_in address;

	// int bind(int sockfd,
	// 		const struct sockaddr *addr, --> Basically, what will the socket be doing, info given in a struct
	// 		socklen_t addrlen); // Why ? The struct will always have a fixed size ?


	address.sin_family = AF_INET; // domaine: IPv4
	address.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
	address.sin_port = htons(_port);

	if (bind(sfd, (struct sockaddr *)&address, sizeof(address)) == -1)
	{ std::cerr << "Error binding socket" << std::endl; throw std::runtime_error("bind"); }

	// So at this step, If There is a conection to our port, it will be sent to the socket

	// Now we need to actively listen for connections
	if (listen(sfd, _max_queue) == -1)
	{ std::cerr << "Error listening on socket" << std::endl; throw std::runtime_error("listen"); }

	// All set, now handle connections
	// For this create an infinite loop that will asynchronously handle connections
	handleConnections(sfd);
}


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

*/
void	serv::handleConnections(int sfd)
{
	std::vector<struct pollfd> fds; // Demo, only one socket
	fds.push_back({sfd, POLLIN, 0});
	int cfd;

	while (_run)
	{
		// Cuz my CPU is burning
		// This goes from 100% CPU usage to 2% CPU usage on the core it's running on.
		sleep(1);

		int		ready = poll(fds.data(), fds.size(), 0);

		if (ready == 0)
			continue; // No events, continue
		if (ready == -1) // Apparently it's bad practice to use errno
		{std::cerr << "Error polling" << std::endl;	throw std::runtime_error("poll");}

		// If we are here, we have an event
		for (size_t i=0; i < fds.size(); ++i)
		{
			if (fds[i].revents & POLLIN)
			{
				std::cout << "\n\nPOLLIN Event on socket " << fds[i].fd << std::endl;

				// Accept the connection
				if (i == 0) // Make a new conection
				{
					struct sockaddr_in client;
					socklen_t client_len = sizeof(client);
					cfd = accept(sfd, (struct sockaddr *)&client, &client_len);
					if (cfd == -1)
					{ std::cerr << "Accept" << std::endl; throw std::runtime_error("accept");}
					fds.push_back({cfd, POLLIN, 0});
					std::cout << "New connection on socket " << cfd << std::endl;
				}
				else // Is already conected, we just need to read
				{
					cfd = fds[i].fd;
					std::cout << "Reusing connection on socket " << cfd << std::endl;
				}

				// Set am 8KB buffer for the socket
				char	socket_buffer[8192] = {0};
				ssize_t	bytes_read = recv(cfd, socket_buffer, sizeof(socket_buffer), MSG_DONTWAIT);
				if (bytes_read > 0)
				{
					std::cout << "Request: {\n" << socket_buffer << "}\n" << std::endl;
					// Do something with the data
					// send back 200 OK
					std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 12\r\n\r\nHello World!";
					if (send(cfd, response.c_str(), response.size(), 0) < 0)
					{ std::cerr << "Error sending response" << std::endl; throw std::runtime_error("send"); }
				}
				else
				{
					std::cerr << "No data received" << std::endl;
					if (i != 0) // The client is done so we close the connection
					{
						std::cout << "Closing connection on socket " << cfd << std::endl;
						close(cfd);
						fds.erase(fds.begin() + i);
					}
				}
			}
		}
	}
}