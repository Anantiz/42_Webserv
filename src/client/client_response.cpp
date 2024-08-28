#include "client.hpp"
#include <string>


bool				Client::end_request( void )
{
	std::map<std::string, std::string>::iterator it = this->request.mainHeader.find("Content-Length");
	if ( it == this->request.mainHeader.end() )
	{
		std::stringstream ss(it->second);
		ss >> this->request.body_size;		
		this->eor = CONTENT_LENGTH;
	}
	std::map<std::string, std::string>::iterator it = this->request.mainHeader.find("Transfer-Encoding");
	if ( it != this->request.mainHeader.end() && it->second == "chunk" )
	{
		this->eor = ENCODING_CHUNK;
	}
	std::map<std::string, std::string>::iterator it = this->request.mainHeader.find("Connection");
	if ( it != this->request.mainHeader.end() && it->second == "close" )
	{
		this->eor = CONNECTION;
	}
	if ( this->request.multipart )
	{
		this->eor = MULTIPART;
	}
}

bool	Client::detect_end( void )
{
	switch (this->eor)
	{
		case CONNECTION:
			return end_connection();
		case CONTENT_LENGTH:
			return end_contentlength();
		case MULTIPART:
			return end_multipart();
		case ENCODING_CHUNK:
			return end_encodingchunk();
	}
}


bool	Client::end_connection( void )
{
	//set variable to aurban
}

bool	Client::end_contentlength( void )
{
	if (this->request.body_size == this->request.received_size)
	{
		return ( true );
	}
	return ( false );
}

bool	Client::end_encodingchunk( void )
{
	if ( this->request.body.find("0\r\n\r\n") != std::string::npos )
		return true;
	return false;
}

bool	Client::end_multipart( void )
{
	if ( this->request.body.find(this->request.boundary.endDelimiter) != std::string::npos )
		return ( true );
}