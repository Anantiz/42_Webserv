#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <cstdlib>
#include <string>
#include <map>
#include <stdexcept>

#include "http/http.hpp"
#include "location/location.hpp"
#include "client/client.hpp"
#include "utils/logs.hpp"
#include "utils/utils.hpp"

#define DEFAULT_ERROR_PAGES_RELATIVE_TO_SERVER "../../http_error_pages/"

class Server {
private:
	std::vector<u_int16_t>			_ports;                 // All ports to which the server listens to
	std::vector<std::string>		_names;                 // DNS names
	enum Http::e_protocol			_protocols;             // 1.0 1.1 2.0, we only handle 1.1 anyway, but can't say we forgot it
	std::vector<Location *>			_locations;
	std::map<int, std::string>		_custom_error_pages;    // If not inside, serve default instead
	size_t							_max_client_body_size;  // -1 for unlimited
	logs							_logger;

	bool _was_set_max_client_body_size;
	bool _was_set_protocol;

public:

	Server();
	~Server();
	void	check_mandatory_directives( void );

	void 							add_port(u_int16_t p);
	void 							add_name(const std::string &n);
	void 							set_protocol(Http::e_protocol p);
	void 							add_location(Location *l);
	void 							add_custom_error_page(int status_code, std::string path);
	void 							set_max_client_body_size(size_t s);

	std::vector<u_int16_t>			&get_ports();
	std::vector<std::string>		&get_names();
	Http::e_protocol				&get_protocol();
	std::vector<Location>			&get_locations();
	std::map<int, std::string>		&get_custom_error_pages();
	size_t							&get_max_client_body_size();

	void							handle_client_request(Client &client);

private:

	Location						&match_best_location(std::string &uri);
	void 							build_error_response(Client &client, int status_code);
};

#endif // SERVER_HPP
