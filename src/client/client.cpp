#include "client.hpp"
#include <string>

bool	gnlEcoplus( std::string &str, std::string &result );
bool	checkline( std::string &line, requestKv	request );
enum	Http::e_method	detectMethode( std::string &method );
enum 	Http::e_protocol	detectProtocol( std::string &proto );


enum ParserState {
    PARSING_HEADERS,
    PARSING_CONTENT,
    LOOKING_FOR_BOUNDARY
};

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
	char		buff[ 1024 ];
	pollfd 		pollFd = getPollfd();
	size_t		totalByte = 0;
	this->state = LOOKING_FOR_BOUNDARY;
	while (true)
	{
		ssize_t	bytes_read = recv(pollFd.fd, buff, sizeof(buffer) - 1, 0);
		if (!bytes_read)
			;
		if ( bytes_read < 0 )
			;
		buffer += buff;
		if ( size_t endPos = (buffer.find("/r/n/r/n") != std::string::npos) )
		{
			std::string line;
			while ( gnlEcoplus( buffer, line ) )
			{
				if (!checkline( line,  this->mainHeader ))
					return false;
			}
			boundaryParser();
		}
		parseChunk();
	}
}

void	Client::parseChunk()
{
	switch ( this->state )
	{
		case LOOKING_FOR_BOUNDARY:
			findBoundary();
			break;
		case PARSING_HEADERS:
			parseHeaders();
			break;
		case PARSING_CONTENT:
			parseContent();
			break;
	}
}

void	Client::findBoundary()
{
	if ()
	{


		state = PARSING_HEADERS;
	}
}


void	Client::parseHeaders()
{
	if ()
	{

		state = PARSING_CONTENT;
	}
}


void	Client::parseContent()
{
	if ()
	{

		state = LOOKING_FOR_BOUNDARY;
	}
}

void	Client::boundaryParser()
{
	std::map<std::string, std::string>::iterator it = this->mainHeader.find("Content-Type");
	std::string	value;
	value = it->second;

	size_t pos = value.find("boundary=");
	std::string	boundary;
	size_t	enPos = value.find( "\r\n" );
	boundary = value.substr(pos + 9, enPos - (pos + 9));
	this->boundary.startDelimiter = "--" + boundary;
	this->boundary.endDelimiter = this->boundary.startDelimiter + "--"; //verifier ces valeurs mais en gros c'est ca
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
