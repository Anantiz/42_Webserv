#include "serv.hpp"

enum e_methods
{
	GET,
	POST,
	DELETE,
	PUT,
	PATCH,
	HEAD,
	OPTIONS,
	TRACE,
	CONNECT
};

// Request:

// GET / HTTP/1.1
// Host: localhost:8080
// User-Agent: curl/7.88.1
// Accept: */*

// Simple struct to have essential data easily accessible
typedef struct s_HttpRequest
{
	enum e_methods method;
	std::string	server;
	std::string path;

	// For other headers (We don't really care about them)
	std::map<std::string, std::string> headers;
}HttpRequest;


void trim(std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if (start == std::string::npos || end == std::string::npos) {
        str.clear();
    } else {
        str = str.substr(start, end - start + 1);
    }
}

// Not very accurate nor safe
void parseRequestPath(std::string &line, HttpRequest &req)
{

	size_t start = line.find_first_of(' ');
	size_t end = line.find_last_of(' ');

	if (start == std::string::npos || end == std::string::npos)
		throw RequestException(400);

	req.path = line.substr(start, end - start);
	if (req.path.empty())
		req.path = "/";
}


int parseRequestMethod(std::string &line, HttpRequest &req)
{
	static const std::string methods[] = \
		{"GET ", "POST ", "DELETE ", "PUT ", "PATCH ", "HEAD ", "OPTIONS ", "TRACE ", "CONNECT ", ""};
	// More performant than using .size() every time.
	static const long methodsLen[] = \
		{3, 4, 6, 4, 6, 5, 8, 6, 8, 0};

	int i = 0;
	long lineLen = line.size();

	while (i < sizeof(methods))
	{
		if (lineLen >= methodsLen[i] && std::strncmp(line.c_str(), methods[i].c_str(), methodsLen[i]) != 0)
		{
			req.method = (e_methods)i;
			if (i > DELETE)
				throw RequestException(501); //501 Not Implemented
			parseRequestPath(line, req);
			return 1;
		}
		++i;
	}
	return 0;
}

/*
	Very naive and basic parser.

	It's just to get an idea of how to parse a request.
*/
HttpRequest &parseRequest(std::string request_str)
{
	int ret;
	std::string line;
	bool method_set = false;

	HttpRequest req;
	std::istringstream request(request_str);

	while (std::getline(request, line))
	{
		trim(line); // Not sure if the HTTP standard allows for spaces before the headers, check this
		size_t cut_index = line.find_first_of(':');
		if (cut_index != std::string::npos)
		{
			std::string key = line.substr(0, cut_index);
			std::string value = line.substr(cut_index + 1);
			if (req.headers.find(key) != req.headers.end())
				throw RequestException(400); // Duplicate header
			req.headers[key] = value;
			continue;
		}

		// Otherwise it's the method line (The only Only one wihout a colon, how convenient :D)
		//Just make a slight adjusment, because the file path can contain semi-colons

		if (!method_set)
		{
			ret = parseRequestMethod(line, req);
			if (ret == 1)
				method_set = true;
			else
				throw RequestException(418); // Invalid method, handle this better (418 to debug the throw location)
		}
		else
			throw RequestException(400); // Duplicate method or just weird header
	}
	if (!method_set)
		throw RequestException(400); // No method
}