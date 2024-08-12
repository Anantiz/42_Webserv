#pragma once
#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <vector>
#include <cstdlib>
#include <string>

#include "http/http.hpp"

class Location {
private:

	bool							_dir_listing;     // If true, list files in dir instead of 404
	bool							_accept_upload;   // Self explanatory
	int								_allowed_methods; // Mask, GET=001 POST=010 DELETE=100
	std::vector<std::string>		_indexes;                 // Default files if uri is a dir
	std::string						_root;                    // Directory where to search the files
	std::string						_upload_dir;              // Directory to save uploaded files, default is root+"/uploads/"
	std::pair<int, std::string>		_redirect;                // (status_code, target_uri)

	// A list of cgi scripts/programs with their respective extensions
	std::vector< std::pair< std::string, std::string> > _cgis;
public:
	Location();
	Location(std::string &location_path);
	~Location();

	void set_dir_listing(bool);
	void set_accept_upload(bool);
	void set_allowed_methods(int);
	void add_index(std::string);
	void set_root(std::string);
	void set_upload_dir(std::string);
	void set_redirect(std::pair<int, std::string>);

	void add_cgi(std::pair<std::string, std::string>);
	void remove_cgi(std::string);

	bool						get_dir_listing();
	bool						get_accept_upload();
	int 						get_allowed_methods();
	std::vector<std::string>	&get_indexes();
	std::string					&get_root();
	std::string					&get_upload_dir();
	std::pair<int, std::string>	&get_redirect();
};

#endif // LOCATION_HPP

/*
	Brain Storm:



*/