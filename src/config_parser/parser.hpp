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

    typedef int (*t_server_directive_ptr)(std::string &line, Server &server);
    typedef int (*t_location_directive_ptr)(std::string &line, Location &location);

	int     fd;
	char    buff[16384];
    size_t  buff_size;
	bool    _fully_read;
	logs 	logger;

    std::map<std::string, t_server_directive_ptr> server_keywords;
    std::map<std::string, t_location_directive_ptr> location_keywords;

	void parse_file( void );
	size_t server_block(const std::string &unit, size_t start);
	size_t location_block(const std::string &unit, size_t start);



    // Server directives
	int listen_directive(std::string &line, Server &server);
	int location_directive(std::string &line, Server &server);
	int server_name_directive(std::string &line, Server &server);
	int error_page_directive(std::string &line, Server &server);
	int client_body_size_directive(std::string &line, Server &server);

	// Location directives
	int root_directive(std::string &line, Location &location);
	int index_directive(std::string &line, Location &location);
	int directory_listing_directive(std::string &line, Location &location);
	int cgi_directive(std::string &line, Location &location);
	int allow_methods_directive(std::string &line, Location &location);
	int return_directive(std::string &line, Location &location);
	int upload_dir_directive(std::string &line, Location &location);

	// Utils

	// A unit is a string starting from the first non-space character
	// to the first semicolon encountered, catches blocks if any
	std::string get_next_unit( void );

	std::vector<Server *> _servers;
	void add_server(Server *server);

public:

	ConfigParser(const char *path);
	~ConfigParser();

	std::vector<Server *>	&get_servers( void );
};

#endif // PARSER_HPP
