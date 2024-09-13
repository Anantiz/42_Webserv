#include "client.hpp"

void				Client::parseChunk()
{
	_logger.SdevLog( "Start parsing the body (chunk case) : " + utils::anything_to_str( this->request.buffer ) );

	/*
	* Recueillir la premiere ligne ( start delimiter )
	* Set GETTING_HEADER_B quand c'est fait
	* Changer get header et parse juste pour mettre dans b_reauest a la place
	* Refaire completement ( parse body classique deja fait)
	* quand nouveau delimiter trouvé -> push back et tej la ligne
	* Quand End delimiter = fin de requete
	*/
	if ( this->b_connection_status == SEARCH_BOUNDARY_FIRST_LINE )
		clean_first_boundary_b();
	if (this->b_connection_status == GETTING_HEADER_B)
		get_header_b();
	if (this->b_connection_status == HEADER_ALL_RECEIVED_B)
		parsefirstheader_b();
	if (this->b_connection_status == GETTING_BODY_B)
		parse_content_b();
}

void				Client::parse_content_b()
{
	_logger.SdevLog( "[Boundary] Parsing body" );

	// because i have to check all the boundary send line by line
	size_t endpos = this->request.buffer.find( "\r\n" );
	if ( endpos != std::string::npos )
	{
		std::string boundary = this->request.buffer.substr( 0, endpos );
		if ( boundary == this->request.boundary.startDelimiter )
		{
			_logger.SdevLog( "[Boundary] Start delimiter found parse header again" );

			this->b_connection_status = SEARCH_BOUNDARY_FIRST_LINE;
			this->request_boundary.push_back( this->b_request );
			this->b_request.body.erase();
			
		}
		else if ( boundary == this->request.boundary.endDelimiter )
		{
			this->connection_status = BODY_ALL_RECEIVED;
			_logger.SdevLog( "[Boundary] End delimiter found end requests" );
			this->request_boundary.push_back( this->b_request );
			this->b_request.body.erase();
			print_request();
			return ;
		}
		else
		{
			this->b_request.body += this->request.buffer.substr(0, endpos + 2);
			this->request.buffer.erase(0, endpos + 2);
		}

	}

}

void	Client::boundaryParser()
{
	std::map<std::string, std::string>::iterator it = this->request.mainHeader.begin();
	while ( it != this->request.mainHeader.end() )
	{
		if (it->first == "Content-Type")
		{
			_logger.SdevLog( "CONTENT LENGTH DETECTED" 	);
			
			size_t pos = it->second.find("boundary=");
			if ( pos != std::string::npos )
			{
				_logger.SdevLog( "CONTENT LENGTH DETECTED" 	);

				this->request.multipart = true;
				size_t	enPos = it->second.find( "\r\n" );
				this->request.boundary.startDelimiter = "--" + it->second.substr(pos + 9, enPos - (pos + 9));
				this->request.boundary.endDelimiter = "--" + it->second.substr(pos + 9, enPos - (pos + 9)) + "--";
			}
		}
		it++;
	}
}

void	Client::parse_header_b()
{
	parsefirstheader_b();
}

bool	Client::get_header_b()
{
		_logger.SdevLog( "[Boundary] in get header" );
		_logger.SdevLog( "[Boundary] buffer to find header : " + utils::anything_to_str( request.buffer ));
	size_t	end_pos = this->request.buffer.find( "\r\n\r\n" );
	if ( end_pos != std::string::npos )
	{
		_logger.SdevLog( "[Boundary] ALL BOUNDARY HEADER RECEIVED" );
		_logger.SdevLog( request.buffer );
		this->b_connection_status = HEADER_ALL_RECEIVED_B;
	}
	return true;
}

void	Client::extract_headers_b( std::string &line )
{
	size_t pos;
	pos = line.find( ':' );
	//There is a ':' and a second part after this that is at least 1 char long
	//Next char after ':' is a space
	if ( (pos != std::string::npos) && (line.size() > (pos + 2)) && (line[pos + 1] == ' '))
	{
		this->b_request.mainHeader[line.substr( 0, pos )] = line.substr( pos + 2, line.size() - 2 );
	}
}

void	Client::parsefirstheader_b()
{
	size_t endpos = this->request.buffer.find( "\r\n\r\n" );
	std::string		first_header = this->request.buffer.substr(0, endpos + 2);
	_logger.SdevLog( "[Boundary] HEADER = " + utils::anything_to_str(first_header) );
	this->request.buffer = this->request.buffer.substr( endpos + 4, this->request.buffer.size() );
	_logger.SdevLog( "[Boundary] Buffer after preextraction = " + utils::anything_to_str(this->request.buffer) );
	std::string 	line;

	while ( gnlEcoplus( first_header,  line ) )
		extract_headers_b( line );

	this->b_connection_status = GETTING_BODY_B;
	//Print all header KEY =   / VALUE = 
	_logger.SdevLog( "[Boundary] HEADER EXTRACTED SUCCESFULLY" );

	std::map<std::string, std::string>::iterator it = b_request.mainHeader.begin();

	while (it != b_request.mainHeader.end())
	{
		_logger.SdevLog( "[Boundary] KEY = " + utils::anything_to_str(it->first) );
		_logger.SdevLog( "[Boundary] VALUE = " + utils::anything_to_str(it->second) );
		it++;
	}
}

void	Client::print_request()
{
	_logger.SdevLog( "\n\n ----------------------- \n REQUEST \n -----------------------");
	_logger.SdevLog( "------------------------------------------------------------------");
	_logger.SdevLog( "Method :" + utils::anything_to_str( this->request.method ) );
	_logger.SdevLog( "Protocol :" + utils::anything_to_str( this->request.protocol ) );
	_logger.SdevLog( "URI :" + utils::anything_to_str( this->request.uri ) );
	_logger.SdevLog( "----------------------- HEADER -----------------------");
	std::map<std::string, std::string>::iterator	it = this->request.mainHeader.begin();
	while ( it != this->request.mainHeader.end())
	{
		_logger.SdevLog( "KEY :" + utils::anything_to_str( it->first ) + " || VALUE :" + utils::anything_to_str( it->second ) );
		it++;
	}
	if ( this->request.multipart )
	{
		_logger.SdevLog( "----------------------- MULTIPART -----------------------");
		std::vector<Http::Request>::iterator	iit = this->request_boundary.begin();
		while ( iit !=  this->request_boundary.end() )
		{
			_logger.SdevLog( "----------------------- NEW PART -----------------------");
			std::map<std::string, std::string>::iterator	itt = iit->mainHeader.begin();
			while ( itt != iit->mainHeader.end() )
			{
				_logger.SdevLog( "KEY :" + utils::anything_to_str( itt->first ) + " || VALUE :" + utils::anything_to_str( itt->second ) );
				itt++;
			}
			_logger.SdevLog( "BODY :" + utils::anything_to_str( iit->body ) );
			iit++;
		}
	}
	else
	{
		_logger.SdevLog( "----------------------- NORMAL -----------------------");

		_logger.SdevLog( "BODY :" + utils::anything_to_str( this->request.body ) );
	}
}