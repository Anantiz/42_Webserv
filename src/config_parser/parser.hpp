#pragma once
#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "server/server.hpp"
#include "cluster/cluster.hpp"
#include "server/location/location.hpp"
#include "utils/logs.hpp"

class ConfigParser
{
private:

    typedef size_t (*t_server_directive_ptr)(const std::string &, size_t , Server &);
    typedef size_t (*t_location_directive_ptr)(const std::string &, size_t , Location &);

    std::map<std::string, t_server_directive_ptr> server_keywords;
    std::map<std::string, t_location_directive_ptr> location_keywords;

	int     fd;
	char    buff[16384];
    size_t  buff_size;
	bool    _fully_read;
	logs 	logger;

	void init_directives_ptr( void );
	void parse_file( void );
	size_t server_block(const std::string &unit, size_t start);
	size_t location_block(const std::string &unit, size_t start);

    // Server directives
	static size_t listen_directive(const std::string &unit, size_t start, Server &server);
	static size_t server_name_directive(const std::string &unit, size_t start, Server &server);
	static size_t error_page_directive(const std::string &unit, size_t start, Server &server);
	static size_t client_body_size_directive(const std::string &unit, size_t start, Server &server);

	// Location directives
	static size_t root_directive(const std::string &unit, size_t start, Location &location);
	static size_t index_directive(const std::string &unit, size_t start, Location &location);
	static size_t directory_listing_directive(const std::string &unit, size_t start, Location &location);
	static size_t cgi_directive(const std::string &unit, size_t start, Location &location);
	static size_t allow_methods_directive(const std::string &unit, size_t start, Location &location);
	static size_t return_directive(const std::string &unit, size_t start, Location &location);
	static size_t upload_dir_directive(const std::string &unit, size_t start, Location &location);

	// Utils

	// A unit is a string starting from the first non-space character
	// to the first semicolon encountered, catches blocks if any
	std::string get_next_unit( void );

	std::vector<Server *> _servers;

public:

	ConfigParser(const char *path);
	~ConfigParser();

	std::vector<Server *>	&get_servers( void );
};

#endif // PARSER_HPP
