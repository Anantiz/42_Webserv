#include "client.hpp"


Client::Client(int poll_fd) {
	// struct s_client_event _data;

	error = 0;
	client_len = sizeof(client_addr);
	int cfd = accept(poll_fd, (struct sockaddr *)&client_addr, &client_len);

	if (cfd == -1)
		throw std::runtime_error("accept");
	this->poll_fd = (pollfd){cfd, POLLIN, 0};
	connection_status = Http::REQUEST;
}

Client::~Client() {
	if (body)
		free(body);
	close(poll_fd.fd);
}

// GETTERS
pollfd &Client::getPollfd() {
	return poll_fd;
}