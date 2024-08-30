#include "client.hpp"
#include <string>

bool	gnlEcoplus( std::string &str, std::string &result );
enum Http::e_method	detectMethod( std::string &method );
enum Http::e_protocol	detectProtocol( std::string &proto );

Client::Client(int arg_poll_fd, int arg_access_port) : client_len(sizeof(client_addr))
{
	// struct s_client_event _data;
	int cfd = accept(arg_poll_fd, (struct sockaddr *)&client_addr, &client_len);
	if (cfd == -1)
		throw std::runtime_error("accept");

	this->poll_fd = (pollfd){cfd, POLLIN, 0};
	this->connection_status = Client::IDLE;
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

// // GETTERS
pollfd &Client::getPollfd() {
	return poll_fd;
}

bool	Client::parse_request()
{
	// std::cout << "one iter" << std::endl;
	char		buff[ 40096 ] = {0};
	pollfd 		pollFd = getPollfd();

	if ( isHeader || isFirstLine )
		this->connection_status = GETTING_HEADER;
	this->request.multipart = false;
	this->state = LOOKING_FOR_BOUNDARY;

	ssize_t	bytes_read = recv(pollFd.fd, buff, sizeof(request.buffer) - 1, 0);
	if (!bytes_read)
		return false;
	if ( bytes_read < 0 )
		return false;
	std::string s = "Client buffer is  = ";
	// _logger.SdevLog( s + buff);
	request.buffer += buff;
	_logger.SdevLog( s + request.buffer );
	/*
	*	Wait to get all the header to parse it -> header ends with "/r/n/r/n"
	*	Check if the content will be multipart
	*	Then check how the request will end, depend of the header's value
	*	Then handle the body
	*/
	size_t endpos = request.buffer.find("\r\n\r\n");
	if (endpos != std::string::npos && isHeader) // This doesn't work so ParseFirstline is never called.
	{
		std::string global_header = request.buffer.substr( 0, endpos );
		_logger.SdevLog( s + global_header);
		request.buffer.erase( 0, endpos + 4 );

		isHeader = false;
		std::string line;
		while ( gnlEcoplus( request.buffer, line ) )
		{
			std::cout << "Header line : " << line << std::endl;
			if ( isFirstLine )
			{
				parseFirstLine( line );
				isFirstLine = false;
			}
			else if (!checkline( line,  this->request.mainHeader ))
				return false;
		}
		this->connection_status = HEADER_ALL_RECEIVED;
		boundaryParser();
		end_request();
	}
	/*
	*	If multipart parse all part with theyre header with parse chunk
	*	Multipart storage is as follow
	*	a list of -> pair(list(header), body)
	*	and in the body. I add content as soon as i get it from the request.
	*/
	if ( !isHeader && this->request.multipart )
	{
		std::pair<std::map<std::string, std::string>, std::string>	headBod;
		parseChunk();
	}
	/*
	*	If not multipart. I only have list(header), body)
	*	the content is add to the body as soon as i get it from the request
	*/
	if ( !isHeader && !this->request.multipart )
	{
		this->connection_status = GETTING_BODY;
		this->request.body += buff;
		this->request.received_size += bytes_read;
		// NOT  SURE ABOUT THAT<

		if (detect_end())
			this->connection_status = BODY_ALL_RECEIVED;
	}
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
		if (this->connection_status == BODY_ALL_RECEIVED)
			return;
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
	{
		state = PARSING_HEADERS;
		this->connection_status = GETTING_HEADER;
	}
}


void	Client::parseHeaders( std::string &line, std::map<std::string, std::string> &headers )
{
	if ( line == "\r\n" )
	{
		this->connection_status = GETTING_BODY;
		this->request.boundary.headBody.push_back(std::make_pair(headers, ""));
		state = PARSING_CONTENT;
	}
	else if ( checkline(line,  headers ) )
		return;
}


void	Client::parseContent( std::string &line )
{
	if ( line.find( this->request.boundary.startDelimiter ) != std::string::npos )
	{
		state = LOOKING_FOR_BOUNDARY;
	}
	else if ( line.find( this->request.boundary.endDelimiter ) != std::string::npos )
	{
		;
	}
	else
		this->request.boundary.headBody.back().second += line;
}

void	Client::boundaryParser()
{
	std::map<std::string, std::string>::iterator it = this->request.mainHeader.find("Content-Type");
	if ( it == this->request.mainHeader.end() )
	{
		this->request.multipart = false;
		return ;
	}
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
	logs::SdevLog("Parse First Line called");
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
	logs::SdevLog("Parse First Line setting attributes");
	this->request.method = detectMethod( methodstr );
	this->request.uri = pathstr;
	// SAFETY NET, LORIS FIX THIS BUG
	// SAFETY NET, LORIS FIX THIS BUG
	if (this->request.uri == "") // Actually that doesn't even work
		this->request.uri = "/";
	// SAFETY NET, LORIS FIX THIS BUG
	// SAFETY NET, LORIS FIX THIS BUG
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
