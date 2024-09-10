#include "client.hpp"
#include <cstring>

Client::Client(int arg_poll_fd, int arg_access_port) : client_len(sizeof(client_addr))
{
	// struct s_client_event _data;
	int cfd = accept(arg_poll_fd, (struct sockaddr *)&client_addr, &client_len);
	if (cfd == -1)
		throw std::runtime_error("accept");

	this->poll_fd = (pollfd){cfd, POLLIN, 0};
	this->connection_status = Client::GETTING_HEADER;
	this->to_close = false;
	this->server = NULL;
	this->access_port = arg_access_port; // To match the server
	this->isHeader = true;
	this->isFirstLine = true;
	this->eor = DONT; // Default value

	this->request.method = Http::GET;
	this->request.buffer = "";
	this->response_status = NONE;
	this->response.status_code = 200; // Default , assume there was no error
}

Client::~Client() {
	close(poll_fd.fd);
}

pollfd &Client::getPollfd() {
	return poll_fd;
}

void	Client::receive_request_data()
{
		pollfd 		pollFd = getPollfd();
		ssize_t	bytes_read = recv(pollFd.fd, buff, sizeof(request.buffer) - 1, 0);
		_logger.SdevLog( "Getting data from fd : " + utils::anything_to_str(poll_fd.fd) );
		if (!bytes_read)
			return ;
		if ( bytes_read < 0 )
			return ;
		_logger.SdevLog( "New content to add : " + utils::anything_to_str(buff) );
		request.buffer.append(buff);
		memset(buff, 0, sizeof(buff));
}

bool Client::parse_request()
{
	receive_request_data();
	if (this->connection_status == GETTING_HEADER)
		get_header();
	if (this->connection_status == HEADER_ALL_RECEIVED)
		parse_header();
	if (this->connection_status == GETTING_BODY)
		parse_content();
	return true;
}

bool	Client::parse_header()
{
	parseFirstLine();
	parsefirstheader();
	set_end_request();
}

bool	Client::get_header()
{
	size_t	end_pos = this->request.buffer.find( "\r\n\r\n" );
	if ( end_pos != std::string::npos )
	{
		_logger.SdevLog( "ALL HEADER RECEIVED" );
		_logger.SdevLog( request.buffer );

		// // factoriser ce bordel (permet de recuperer ce quón recoit en plus du header une fois le header recu)
		// size_t length = this->request.buffer.length() - (end_pos + 4);
		// if (length > sizeof(this->buff) - 1) 
		// {
    	// 	length = sizeof(this->buff) - 1;  // Leave space for the null terminator
		// }
		// // Copy the substring from request.buffer into this->buff
		// this->request.buffer.copy(this->buff, length, end_pos + 4);

		// // Null-terminate the buffer
		// this->buff[length] = '\0';
		this->connection_status = HEADER_ALL_RECEIVED;
	}
	return true;
}
