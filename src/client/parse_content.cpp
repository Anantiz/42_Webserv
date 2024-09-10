#include "client.hpp"
#include <cstring>  // for strlen

void				Client::parseChunk()
{
	Http::Request	b_request;
	size_t end_line_pos = this->request.buffer.find( "\r\n" );
	if ( end_line_pos != std::string::npos )
	{
		//detect the limiter -> handle header and content like usual (redo all function for multi)
		this->request_boundary.push_back(b_request);
	}
}

void				Client::parseBody()
{
	_logger.SdevLog( "Start parsing the body (default case) : " + utils::anything_to_str( this->request.buffer ) );
	this->request.body.append(this->request.buffer);
	this->request.received_size += this->request.buffer.size();
	bzero( buff, strlen(buff) );
	this->request.buffer.erase();

	_logger.SdevLog( "Total body : " + utils::anything_to_str( this->request.body ) );
	_logger.SdevLog( "Content lenght : " + utils::anything_to_str( this->request.received_size ) );
	_logger.SdevLog( "Supposed size : " + utils::anything_to_str( this->request.body_size ) );
	if ( detect_end() == true )
	{
		_logger.SdevLog( "End detected" );
		this->connection_status = BODY_ALL_RECEIVED;
	}
}


bool				Client::parse_content()
{
	_logger.SdevLog( "Start parsing the content (select chunk of default)" );
	if (this->eor == MULTIPART)
		parseChunk();
	else
		parseBody();
}