#include "client.hpp"

enum Http::e_protocol	detectProtocol( std::string &proto )
{
	std::string base;
	base = proto.substr(0, 5);
	
	if ( base != "HTTP/" || !std::isdigit(proto[5]) || !(proto[6] == '.') || !(std::isdigit(proto[7])))
		return Http::FALSE_PROTOCOL;
	if (proto == "HTTP/1.0")
		return Http::HTTP_1_0;
	else if ( utils::anything_to_str(proto) == "HTTP/1.1" )
		return Http::HTTP_1_1;
	else if ( proto == "HTTP/2.0" )
		return Http::HTTP_2_0;
	else
		return Http::Others;
}

enum Http::e_method	detectMethod( std::string &method )
{
	if (method == "GET")
		return Http::GET;
	else if ( method == "POST" )
		return Http::POST;
	else if ( method == "DELETE" )
		return Http::DELETE;
	else
		return Http::UNKNOWN_METHOD;
}

std::string		Client::getFirstLine()
{
	size_t endpos = this->request.buffer.find( "\r\n" );
	std::string ret = this->request.buffer.substr(0, endpos);
	this->request.buffer.erase( 0, endpos + 2 );
	return ret;
}

void	Client::parseFirstLine()
{
	std::string first_line = getFirstLine();

	size_t firstSpace = first_line.find(' ');
	// if (firstSpace == std::string::npos) return false;

    size_t secondSpace = first_line.find(' ', firstSpace + 1);
    // if (secondSpace == std::string::npos) return false;

	std::string methodstr = first_line.substr(0, firstSpace);
    std::string pathstr = first_line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    std::string protocolstr = first_line.substr(secondSpace + 1);

	this->request.method = detectMethod( methodstr );
	this->request.uri = pathstr;
	this->request.protocol = detectProtocol( protocolstr );


	_logger.SdevLog( "HEADER FIRST LINE PARSED SUCCESFULLY" );
	_logger.SdevLog( "Method = " + utils::ito_str(this->request.method));
	_logger.SdevLog( "Uri = " + utils::anything_to_str((this->request.uri)));
	_logger.SdevLog( "Protocol = " + utils::ito_str(this->request.protocol));
}

bool	gnlEcoplus( std::string &str, std::string &result )
{
	size_t pos;
	if ( (pos = str.find( "\r\n" )) != std::string::npos )
	{	if ( pos == 0 )
			return ( false );
		std::string line = str.substr( 0, pos );
		str.erase( 0, pos + 2 );
		result = line;
		return ( true );
	}
	return false;
}
void	Client::extract_headers( std::string &line )
{
	size_t pos;
	pos = line.find( ':' );
	//There is a ':' and a second part after this that is at least 1 char long
	//Next char after ':' is a space
	if ( (pos != std::string::npos) && (line.size() > (pos + 2)) && (line[pos + 1] == ' '))
	{
		this->request.mainHeader[line.substr( 0, pos )] = line.substr( pos + 2, line.size() - 2 );
	}
}

void	Client::parsefirstheader()
{
	size_t endpos = this->request.buffer.find( "\r\n\r\n" );
	std::string		first_header = this->request.buffer.substr(0, endpos + 2);
	_logger.SdevLog( "HEADER = " + utils::anything_to_str(first_header) );
	this->request.buffer = this->request.buffer.substr( endpos + 4, this->request.buffer.size() );
	_logger.SdevLog( "Buffer after preextraction = " + utils::anything_to_str(this->request.buffer) );
	std::string 	line;
	while ( gnlEcoplus( first_header,  line ) )
		extract_headers( line );


	set_end_request();
	boundaryParser();
	_logger.SdevLog( " Is it multipart ? : " + utils::anything_to_str(this->request.multipart) );

	//Print all header KEY =   / VALUE = 
	_logger.SdevLog( "HEADER EXTRACTED SUCCESFULLY" );

	std::map<std::string, std::string>::iterator it = this->request.mainHeader.begin();

	while (it != this->request.mainHeader.end())
	{
		_logger.SdevLog( "KEY = " + utils::anything_to_str(it->first) );
		_logger.SdevLog( "VALUE = " + utils::anything_to_str(it->second) );
		it++;
	}
}