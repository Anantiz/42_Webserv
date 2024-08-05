#pragma once
#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <vector>
#include <cstdlib>
#include <string>

#include "http/http.hpp"

class Location {
private:

	bool							_dir_listing = false;     // If true, list files in dir instead of 404
	bool							_accept_upload = false;   // Self explanatory
	int								_allowed_methods = 0b011; // Mask, GET=001 POST=010 DELETE=100
	std::vector<std::string>		_indexes;                 // Default files if uri is a dir
	std::string						_root;                    // Directory where to search the files
	std::string						_upload_dir;              // Directory to save uploaded files, default is root+"/uploads/"
	std::pair<int, std::string>		_redirect;                // (status_code, target_uri)

	// A list of cgi scripts/programs with their respective extensions
	std::vector< std::pair< std::string, std::string> > _cgis;
};

#endif // LOCATION_HPP

/*
	Brain Storm:



*/