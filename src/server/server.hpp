#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <cstdlib>
#include <string>

#include "http/http.hpp"
#include "location/location.hpp"

class Server {
private:
	std::vector<u_int16_t>			_ports;                 // All ports to which the server listens to
	std::vector<std::string>		_names;                 // DNS names
	enum Http::e_protocol			_protocols;             // 1.0 1.1 2.0, we only handle 1.1 anyway, but can't say we forgot it
	std::vector<Location>			_locations;
	std::map<int, std::string>		_custom_error_pages;    // If not inside, serve default instead
	size_t							_max_client_body_size;  // -1 for unlimited
};

#endif // SERVER_HPP


