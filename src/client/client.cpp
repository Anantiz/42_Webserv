#include "client.hpp"
#include <string>

bool	gnlEcoplus( std::string &str, std::string &result );
bool	checkline( std::string &line, requestKv	request );
enum	Http::e_method	detectMethode( std::string &method );
enum 	Http::e_protocol	detectProtocol( std::string &proto );


Client::Client(int poll_fd) {
	// struct s_client_event _data;
	client_len = sizeof(client_addr);
	int cfd = accept(poll_fd, (struct sockaddr *)&client_addr, &client_len);
	if (cfd == -1)
		throw std::runtime_error("accept");
	this->poll_fd = (pollfd){cfd, POLLIN, 0};
	connection_status = Http::REQUEST;
}

Client::~Client() {
	// if (body)
	// 	free(body);
	close(poll_fd.fd);
}

// // GETTERS
pollfd &Client::getPollfd() {
	return poll_fd;
}

bool	Client::parse_request()
{
	requestKv 	rKeyVal;
	bool		isHeader = true;
	bool		isFirstLine = false;
	char		buffer[ 1024 ];
	pollfd 		pollFd = getPollfd();
	size_t		totalByte = 0;
	while (true)
	{
		ssize_t	bytes_read = recv(pollFd.fd, buffer, sizeof(buffer) - 1, 0);
		totalByte += bytes_read;
		if ( bytes_read < 0 )
			; //handle error
		else if ( bytes_read == 0 )
			break; //handle end or no info
		else if ( isHeader )
		{
			buffer[ bytes_read ] = '\0';
			std::string str( buffer );
			std::string	header;
			header += str;
			std::string startBody;
			if ( size_t endPos = (header.find( "\r\n\r\n" ) != std::string::npos) )
			{
				if ( (endPos + 4) < totalByte )
					startBody = header.substr((endPos + 4), totalByte - (endPos + 4));
				isHeader = false;
				if ( size_t pos = (str.find("boundary=") != std::string::npos) )
				{
					this->multipart = true;
					std::string delimiter = boundaryExtractor( header, pos + 8 );
					this->boundary.endDelimiter = delimiter + "--";
					this->boundary.startDelimiter = "--" + delimiter;
				}
				std::string	line;
				while ( gnlEcoplus( header, line ) )
				{
					if ( isFirstLine )
					{
						isFirstLine = false;
						if ( !parseFirstLine( line ) )
							return false;
					}
					else
					{
						if (!checkline( line, rKeyVal ))
							return false;
						if ( rKeyVal.key == "Host" )
							this->request.host = rKeyVal.value;
						else
							this->request.headers[rKeyVal.key] = rKeyVal.value;
					}
				}
			}
			// handle body
		}
	}
	return true;
}

std::string	boundaryExtractor( std::string &str, size_t pos)
{
	size_t endPos = str.find( "\r\n", pos );
	std::string	boundary = str.substr( pos, endPos - pos );
	return boundary;
}

void	boundaryParser( std::string &str )
{

}

bool isAlpha( const std::string& str ) 
{
    for ( size_t i = 0; i < str.size(); ++i ) 
	{
        if ( !std::isalnum(str[i]) && str[i] != '.' && str[i] != '/')
            return false;
    }
    return true;
}

bool	Client::parseFirstLine( std::string &line )
{
	size_t firstSpace = line.find(' ');
	if (firstSpace == std::string::npos) return false;

    size_t secondSpace = line.find(' ', firstSpace + 1);
    if (secondSpace == std::string::npos) return false;

	if (line.find(' ', secondSpace + 1) != std::string::npos) return false;


	std::string methodstr = line.substr(0, firstSpace);
    std::string pathstr = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    std::string protocolstr = line.substr(secondSpace + 1);

   	if (!isAlpha(methodstr) || !isAlpha(pathstr) || !isAlpha(protocolstr))
        return false;
	this->request.method = detectMethode( methodstr );
	this->request.uri = pathstr;
	this->request.protocol = detectProtocol( protocolstr );
	if (this->request.method == Http::UNKNOWN_METHOD || this->request.protocol == Http::FALSE_PROTOCOL)
		return false;
	return true;
}
enum Http::e_protocol	detectProtocol( std::string &proto )
{
	std::string base;
	base = proto.substr(4);
	if ( base == "HTTP/" && std::isdigit(proto[5]) && proto[6] == '.' && std::isdigit(proto[7]))
		return Http::FALSE_PROTOCOL;
	if (proto == "HTTP/1.0")
		return Http::HTTP_1_0;
	else if ( proto == "HTTP/1.1" )
		return Http::HTTP_1_1;
	else if ( proto == "HTTP/2.0" )
		return Http::HTTP_2_0;
	else
		return Http::Others;
}

enum Http::e_method	detectMethode( std::string &method )
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

// Get next line eco+ for std::string
bool	gnlEcoplus( std::string &str, std::string &result )
{
	size_t pos;
	if ( (pos = str.find( "\n\r" )) != std::string::npos )
	{
		std::string line = str.substr( 0, pos );
		str.erase( 0, pos + 1 );
		result = line;
		return ( true );
	}
	return false;
}



// Detec type of every line of the request
bool	checkline( std::string &line, requestKv	request )
{
	size_t pos;
	if ( (pos = line.find( ':' )) != std::string::npos && line.size() < pos + 2 )
	{
		if (line[pos + 1] != ' ')
			return false;
		request.key = line.substr( 0, pos );
		request.value = line.substr( pos + 2, line.size() );
		return true;
	}
	else
		return false;
}
