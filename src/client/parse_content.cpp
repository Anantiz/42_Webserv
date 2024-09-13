#include "client.hpp"

void				Client::clean_first_boundary_b()
{
	size_t endpos = this->request.buffer.find( "\r\n" );
	if ( endpos != std::string::npos )
	{
		std::string boundary = this->request.buffer.substr( 0, endpos );
		if ( boundary == this->request.boundary.startDelimiter )
		{
		_logger.SdevLog( "Delimiter found start partsing header" );

			this->b_connection_status = GETTING_HEADER_B;
			this->request.buffer.erase( 0, endpos + 2 );
		}
	}
	//else error
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
		print_request();
	}
}


void				Client::parse_content()
{
	_logger.SdevLog( "Start parsing the content (select chunk of default)" );
	if (this->eor == MULTIPART)
		parseChunk();
	else
		parseBody();
}