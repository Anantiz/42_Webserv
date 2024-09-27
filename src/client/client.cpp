#include "client.hpp"
#include <cstring>

Client::Client(int entry_socket_fd, int arg_access_port) : client_len(sizeof(client_addr))
{
	// struct s_client_event _data;
	int cfd = accept(entry_socket_fd, (struct sockaddr *)&client_addr, &client_len);
	if (cfd == -1)
		throw std::runtime_error("Runtime error: " + std::string(::strerror(errno)));

	memset(buff, 0, sizeof(buff));

	this->poll_fd = (pollfd){cfd, POLLIN, 0};
	this->connection_status = Client::GETTING_HEADER;
	this->access_port = arg_access_port; // To match the server
	this->server = NULL;
	this->to_close = true;
	this->b_connection_status = Client::SEARCH_BOUNDARY_FIRST_LINE;
	this->response_status = NONE;

	this->isHeader = true;
	this->isFirstLine = true;
	this->eor = DONT; // Default value
	this->isFirstLine_b = true;
	// this->request.method = Http::GET;

    this->response.file_fd = -1;
    this->request.method = Http::UNKNOWN_METHOD;
    this->request.protocol = Http::FALSE_PROTOCOL;
    this->request.host.clear();
    this->request.uri.clear();
    this->request.headers.clear();
    this->request.body_size = 0;
    this->request.received_size = 0;
    this->request.body.clear();
    this->request.mainHeader.clear();
    this->request.boundary.startDelimiter.clear();
    this->request.boundary.endDelimiter.clear();
    this->request.boundary.headBody.clear();
    this->request.buffer.clear();
    this->request.multipart = false;

	this->request.keep_alive = false;
	this->request.content_length = 0;
	this->request.content_type.clear();

	this->response.method = Http::UNKNOWN_METHOD;
    this->response.status_code = 200;
    this->response.headers.clear();
    this->response.file_path_to_send.clear();
    this->response.body.clear();
    this->response.body_size = 0;
    this->response.buffer.clear();
    this->response.last_read = 0;
    this->response.offset = 0;

}

Client::~Client() {
	close(poll_fd.fd);
}

pollfd &Client::getPollfd() {
	return poll_fd;
}

const size_t MAX_BUFFER_SIZE = 32 * 1024 * 1024; // 32 * 1024 * 1024 = 33,554,432 bytes (32 MB)

void addToBuffer(std::string &buffer, const char *data) {
    if (buffer.size() > MAX_BUFFER_SIZE) {
        throw Http::HttpException(414);
    }
    buffer.append(data);
}

void	Client::receive_request_data()
{
		pollfd 		pollFd = getPollfd();
		ssize_t	bytes_read = recv(pollFd.fd, buff, sizeof(request.buffer) - 1, 0);
		_logger.SdevLog( "Getting data from fd : " + utils::anything_to_str(poll_fd.fd) );
		if ( bytes_read <= 0 ) {
			_logger.SdevLog( "No data");
			this->connection_status = Client::TO_CLOSE;
			this->to_close = true;
			return ;
		}
		// _logger.SdevLog( "New content to add : " + utils::anything_to_str(buff) );
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

void	Client::parse_header()
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
		// _logger.SdevLog( request.buffer );
		this->connection_status = HEADER_ALL_RECEIVED;
	}
	return true;
}
