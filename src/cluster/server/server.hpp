#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <cstdlib>
#include <string>

#include "location/location.hpp"

class Server {
private:
	std::vector<u_int16_t>			_ports;
	std::vector<std::string>		_names;
	std::vector<Location>			_locations;
};


#endif // SERVER_HPP
