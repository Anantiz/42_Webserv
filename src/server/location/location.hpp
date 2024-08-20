#pragma once
#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <vector>
#include <cstdlib>
#include <string>
#include <dirent.h>
#include <sys/types.h>

#include "http/http.hpp"
#include "utils/utils.hpp"
#include "client/client.hpp"

class Location {
private:

	std::string						_location_path;   // Path to match with uri
	std::string						_root;            // Directory where to search the files
	std::string						_upload_dir;      // Directory to save uploaded files, default is root+"/uploads/"
	std::vector<std::string>		_indexes;         // Default files if uri is a dir

	bool							_dir_listing;     // If true, list files in dir instead of 404
	bool							_accept_upload;   // Self explanatory
	int								_allowed_methods; // Mask, GET=001 POST=010 DELETE=100
	std::pair<int, std::string>		_redirect;        // (status_code, target_uri)

	std::vector< std::pair< std::string, std::string> > _cgis; // A list of cgi scripts/programs with their respective extensions

	bool _was_set_root;
	bool _was_set_upload_dir;
	bool _was_set_dir_listing;
	bool _was_set_accept_upload;
	bool _was_set_allowed_methods;
	bool _was_set_redirect;

public:
	Location(std::string &location_path);
	~Location();
	void check_mandatory_directives( void );

	// setters
	void						set_dir_listing(bool);
	void						set_accept_upload(bool);
	void						set_allowed_methods(int);
	void						add_index(std::string);
	void						set_root(std::string);
	void						set_upload_dir(std::string);
	void						set_redirect(std::pair<int, std::string>);
	void						add_cgi(std::pair<std::string, std::string>);

	// getters
	bool						get_dir_listing();
	bool						get_accept_upload();
	int							get_allowed_methods();
	std::vector<std::string>	&get_indexes();
	std::string					&get_root();
	std::string					&get_upload_dir();
	std::pair<int, std::string>	&get_redirect();

	// Methods
	void						build_request_response(Client &client);
	size_t						count_blocks(std::string &uri);

private:

	void						handle_get_request(Client &client);
	void						handle_post_request(Client &client);
	void						handle_delete_request(Client &client);

	// Get
	std::string					get_local_path(std::string &uri);
	std::string					dir_listing_content(const std::string &dir_path );
	void						build_response_get_file(Client &client, std::string &local_path);
	void						build_response_get_dir(Client &client, std::string &local_path);

	// Post
	void						download_client_file(Client &client, std::string &file_path);
	void						post_to_cgi(Client &client, std::string &local_path);

	// Delete
	void						delete_file(std::string &file_path);


};

#endif // LOCATION_HPP