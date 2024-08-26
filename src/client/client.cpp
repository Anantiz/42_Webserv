#include "client.hpp"
#include <string>

Client::Client(int poll_fd) : client_len(sizeof(client_addr))
{
	// struct s_client_event _data;
	int cfd = accept(poll_fd, (struct sockaddr *)&client_addr, &client_len);
	if (cfd == -1)
		throw std::runtime_error("accept");
	this->poll_fd = (pollfd){cfd, POLLIN, 0};
	connection_status = Client::IDLE;
	to_close = false;
}

Client::~Client() {
	close(poll_fd.fd);
}

// // GETTERS
pollfd &Client::getPollfd() {
	return poll_fd;
}

bool	Client::parse_request()
{
	requestKv request;
	int		isHeader = 0;
	char	buffer[ 1024 ];
	pollfd pollFd = getPollfd();
	ssize_t	bytes_read = recv(poll_fd.fd, buffer, sizeof(buffer) - 1, 0);
	if ( bytes_read < 0 )
		; //handle error
	else if ( bytes_read == 0 )
		; //handle end or no info
	else
	{
		buffer[ bytes_read ] = '\0';
		std::string str( buffer );
		std::string	line;
		while ( gnlEcoplus( str, line ) )
		{
			if ( !isHeader )
			{
				isHeader++;
				std::string methodstr;
				std::string pathstr;
				std::string protocolstr;
				if (!parseFirstLine(line, methodstr, pathstr, protocolstr))
					return false;
			}
			else if ( isHeader == 1 )
			{
				if (!checkline( line, request ));
					return false;
				request.key == "Host";
				this->request.host = request.value;
			}
			else
			{
				if (!checkline( line, request ));
					return false;
				this->request.body.append(line);
			}
		}
	}
	return true;
}

bool isAlpha( const std::string& str )
{
    for ( size_t i = 0; i < str.size(); ++i )
	{
        if ( !std::isalnum(str[i]) && str[i] != '.' && str[i] != '/ ')
            return false;
    }
    return true;
}

bool	Client::parseFirstLine( std::string &line, std::string &methodstr,
					std::string &pathstr,
					std::string &protocolstr )
{
	size_t firstSpace = line.find(' ');
	if (firstSpace == std::string::npos) return false;

    size_t secondSpace = line.find(' ', firstSpace + 1);
    if (secondSpace == std::string::npos) return false;

	if (line.find(' ', secondSpace + 1) != std::string::npos) return false;


	methodstr = line.substr(0, firstSpace);
    pathstr = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    protocolstr = line.substr(secondSpace + 1);

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
	if ( pos = str.find( "\n\r" ) != std::string::npos )
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
	if ( pos = line.find( ':' ) != std::string::npos && line.size() < pos + 2 )
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