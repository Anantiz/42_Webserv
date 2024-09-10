#include "client.hpp"

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
				this->request.boundary.endDelimiter = it->second.substr(pos + 9, enPos - (pos + 9)) + "--";
			}
		}
		it++;
	}
}