#include "client_event.hpp"


ClientEvent::ClientEvent(int poll_fd) {
	struct s_client_event event;

	event.client_len = sizeof(event.client_addr);
	int cfd = accept(poll_fd, (struct sockaddr *)&event.client_addr, &event.client_len);

	if (cfd == -1)
		throw std::runtime_error("accept");
	event.pollfd = (pollfd){cfd, POLLIN, 0};
	event.connection_status = CEvent::REQUEST;
	_request = event;
}

ClientEvent::~ClientEvent() {
	if (_request.body)
		free(_request.body);
	close(_request.pollfd.fd);
}

// GETTERS

s_client_event &ClientEvent::getRequest() {
	return _request;
}

pollfd &ClientEvent::getPollfd() {
	return _request.pollfd;
}