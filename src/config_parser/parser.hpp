#pragma once
#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <fstream>
#include <string>

#include "server/server.hpp"
#include "cluster/cluster.hpp"
#include "server/location/location.hpp"


class ConfigParser
{
private:

	std::string		_buffer;

	void global_block();
	void server_block();
	void location_block();

public:

	ConfigParser(const char *path);
	~ConfigParser();
};

#endif // PARSER_HPP
