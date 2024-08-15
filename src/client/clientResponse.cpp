#include "client.hpp"

void	Client::getMethod( void )
{
	std::string	file_path = this->request.uri;
	if ( this->request.uri == "/" )
		file_path += "index.html";

	std::ifstream file(file_path.c_str());
	if (!file.is_open())
		;//handleerror
	std::ostringstream ss;
	ss << file.rdbuf();
	std::string	body = ss.str();
	file.close();
}