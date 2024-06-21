#pragma once

#include <iostream>
#include <sstream>
#include <cstring> // Safe Comparaison
#include <string>
#include <stdexcept>

#include <sys/socket.h>
#include <sys/poll.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <vector>
#include <map>

class serv
{
public:
	serv(std::string name, short port, std::string root) : _name(name), _port(port), _root(root) {}
	~serv() {}

	void start();
	void handleConnections(int sfd);

private:

	std::string _name;
	short _port;
	std::string _root;

	bool	_run = true;
	int 	_max_queue = 10;
	int		_max_events = 50;

	int		_events_count = 0;

};

class RequestException : public std::exception
{
private:
	const int _status;

public:
	RequestException(const int status) : _status(status) {}
	~RequestException() throw() {}

	const int	status() const { return _status; }
	const char *what() const throw() { return "Bad Request"; };
};

// Example for this exception:

/*
	try
	{
		throw RequestException(400);
	}
	catch (const RequestException &e)
	{
		return (get_error_page(e.status()));
	}
*/