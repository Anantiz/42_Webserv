#include "client.hpp"
#include <string>

bool	gnlEcoplus( std::string &str, std::string &result );
enum Http::e_method	detectMethode( std::string &method );
enum Http::e_protocol	detectProtocol( std::string &proto );

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
	this->request.multipart = false;
	// requestKv 	rKeyVal;
	bool		isHeader = true;
	// bool		isFirstLine = false;
	char		buff[ 212992 ];
	pollfd 		pollFd = getPollfd();
	// size_t		totalByte = 0;
	this->state = LOOKING_FOR_BOUNDARY;
	// while (true)
	// {
		ssize_t	bytes_read = recv(pollFd.fd, buff, sizeof(request.buffer) - 1, 0);
		if (!bytes_read)
			;
		if ( bytes_read < 0 )
			;
		request.buffer += buff;

		if ( size_t endPos = ( request.buffer.find( "/r/n/r/n" ) != std::string::npos ) && isHeader)
		{
			isHeader = false;
			std::string line;
			while ( gnlEcoplus( request.buffer, line ) )
			{
				if (!checkline( line,  this->request.mainHeader ))
					return false;
			}
			this->connection_status = HEADER_ALL_RECEIVED;
			boundaryParser();
		}
		if ( !isHeader && this->request.multipart )
		{
			std::pair<std::map<std::string, std::string>, std::string>	headBod;
			parseChunk();
		}
		if ( !isHeader && !this->request.multipart )
		{
			this->request.body += buff;
			this->request.body_size += bytes_read;
		}
	// }
	return true;
}

bool	Client::isLine()
{
	if ( this->request.buffer.find( "\r\n" ) != std::string::npos )
		return true;
	return false;
}

void	Client::parseChunk()
{
	std::string	line;
	std::map<std::string, std::string> headers;
	std::string body;
	while ( isLine() )
	{
		line = request.buffer.substr(0, ( request.buffer.find( "\r\n" ) + 2 ));
		request.buffer.erase(0, request.buffer.find( "\r\n" ) + 2 );
		switch ( this->state )
		{
			case LOOKING_FOR_BOUNDARY:
				findBoundary( line );
				break;
			case PARSING_HEADERS:
				parseHeaders( line, headers  );
				break;
			case PARSING_CONTENT:
				parseContent( line );
				break;
		}
	}
}

void	Client::findBoundary( std::string &line )
{
	if ( line.find( this->request.boundary.startDelimiter ) != std::string::npos )
		state = PARSING_HEADERS;
}


void	Client::parseHeaders( std::string &line, std::map<std::string, std::string> &headers )
{

	if ( line == "\r\n" )
	{
		this->request.boundary.headBody.push_back(std::make_pair(headers, ""));
		state = PARSING_CONTENT;
	}
	else if ( checkline(line,  headers ) )
		return;
}


void	Client::parseContent( std::string &line )
{
	if ( line.find( this->request.boundary.endDelimiter ) != std::string::npos )
	{
		state = LOOKING_FOR_BOUNDARY;
	}
	else
		this->request.boundary.headBody.back().second += line;
}

void	Client::boundaryParser()
{
	std::map<std::string, std::string>::iterator it = this->request.mainHeader.find("Content-Type");
	if ( it == this->request.mainHeader.end() )
		return ;
	std::string	value;
	value = it->second;
	this->request.multipart = true;
	size_t pos = value.find("boundary=");
	std::string	boundary;
	size_t	enPos = value.find( "\r\n" );
	boundary = value.substr(pos + 9, enPos - (pos + 9));
	this->request.boundary.startDelimiter = "--" + boundary;
	this->request.boundary.endDelimiter = this->request.boundary.startDelimiter + "--"; //verifier ces valeurs mais en gros c'est ca
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
bool	Client::checkline( std::string &line, std::map<std::string, std::string>	&intermheader )
{
	size_t pos;
	if ( (pos = line.find( ':' )) != std::string::npos && line.size() < pos + 2 )
	{
		if (line[pos + 1] != ' ')
			return false;
		intermheader[line.substr( 0, pos )] = line.substr( pos + 2, line.size() );
		return true;
	}
	else
		return false;
}
