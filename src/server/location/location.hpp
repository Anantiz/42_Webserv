#pragma once
#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <vector>
#include <cstdlib>
#include <string>

#include "http/http.hpp"

class Location {
private:

	bool							_dir_listing = false;
	bool							_accept_upload = false;
	bool							_cgi_on = false;
	int								_allowed_methods = 0b001; // Mask, GET=001 POST=010 DELETE=100
	std::vector<std::string>		_indexes;                 // Default files if uri is a dir
	std::string						_root;                    // Directory where to search the files
	std::string						_cgi_extension;           // sends to cgi if extension match, none means any file
	std::string						_cgi_path;                // path to execute
};

#endif // LOCATION_HPP

/*
	Brain Storm:


*/