#include "location.hpp"

Location::Location(std::string &location_path) : _location_path(location_path)
{
    _dir_listing = false;
    _accept_upload = false;
    _allowed_methods = 0b011;
    _root = "";
    _upload_dir = "";
   	_was_set_root = false;
	_was_set_upload_dir = false;
	_was_set_dir_listing = false;
	_was_set_accept_upload = false;
	_was_set_allowed_methods = false;
    _was_set_redirect = false;
}

Location::~Location()
{
}

void Location::check_mandatory_directives( void )
{
    if (_root.empty())
        throw std::runtime_error("No root specified for location");
}


void Location::set_dir_listing(bool b)
{
    if (_was_set_dir_listing)
        throw std::runtime_error("Duplicate directive: directory_listing");
    _was_set_dir_listing = true;
    _dir_listing = b;
}

void Location::set_accept_upload(bool b)
{
    if (_was_set_accept_upload)
        throw std::runtime_error("Duplicate directive: accept_upload");
    _was_set_accept_upload = true;
    _accept_upload = b;
}

void Location::set_allowed_methods(int m)
{
    if (_was_set_allowed_methods)
        throw std::runtime_error("Duplicate directive: allow_methods");
    _was_set_allowed_methods = true;
    _allowed_methods = m;
}

void Location::add_index(const std::string& i)
{
    _indexes.push_back(i);
}

void Location::set_root(const std::string& r)
{
    if (_was_set_root)
        throw std::runtime_error("Duplicate directive: root");
    _was_set_root = true;
    _root = r;
    if (_root[_root.size() - 1] != '/')
        _root += "/";

    if (_upload_dir.empty())
        _upload_dir = r + "/uploads/";
}

void Location::set_upload_dir(const std::string& u)
{
    if (_was_set_upload_dir)
        throw std::runtime_error("Duplicate directive: upload_dir");
    _was_set_upload_dir = true;
    _upload_dir = u;
}

void Location::set_redirect(std::pair<int, const std::string > r)
{
    if (_was_set_redirect)
        throw std::runtime_error("Duplicate directive: return");
    _was_set_redirect = true;
    _redirect = r;
}

void Location::add_cgi(std::pair<const std::string, const std::string > c)
{
    _cgis.push_back(c);
}

bool Location::get_dir_listing()
{
    return _dir_listing;
}

bool Location::get_accept_upload()
{
    return _accept_upload;
}

int Location::get_allowed_methods()
{
    return _allowed_methods;
}

std::vector<std::string> &Location::get_indexes()
{
    return _indexes;
}

std::string &Location::get_root()
{
    return _root;
}

std::string &Location::get_upload_dir()
{
    return _upload_dir;
}

std::pair<int, std::string> &Location::get_redirect()
{
    return _redirect;
}

/*
██████  ██    ██ ██████  ██      ██  ██████
██   ██ ██    ██ ██   ██ ██      ██ ██
██████  ██    ██ ██████  ██      ██ ██
██      ██    ██ ██   ██ ██      ██ ██
██       ██████  ██████  ███████ ██  ██████
*/


/*
    Count the number of blocks in common with this location
*/
size_t  Location::count_blocks(const std::string &uri) const
{
    size_t  count = 0;
    size_t  i = 0;

    // Just learned today that cpp support english words for logical operators
    while (i < uri.size() and i < _location_path.size())
    {
        if (_location_path[i] != uri[i])
            break;
        if (uri[i] == '/')
            count++;
        i++;
    }
    return count;
}

void   Location::build_request_response(Client &client)
{
    if ((_allowed_methods & client.request.method) == 0)
        throw Http::HttpException(405);
    switch (client.request.method)
    {
        case Http::GET:
            logs::SdevLog("Handling GET request");
            handle_get_request(client);
            break;
        case Http::POST:
            logs::SdevLog("Handling POST request");
            handle_post_request(client);
            break;
        case Http::DELETE:
            logs::SdevLog("Handling DEL request");
            handle_delete_request(client);
            break;
        default:
            throw Http::HttpException(501);
    }
}

/*
██████  ██████  ██ ██    ██  █████  ████████ ███████
██   ██ ██   ██ ██ ██    ██ ██   ██    ██    ██
██████  ██████  ██ ██    ██ ███████    ██    █████
██      ██   ██ ██  ██  ██  ██   ██    ██    ██
██      ██   ██ ██   ████   ██   ██    ██    ███████
*/

std::string   Location::get_local_path(std::string &uri)
{
	// uri            = /image/chats/1.jpg
	// _location_path = /image/chats
	// _root          = /var/www/images
	// local_path     = root + (uri - location_path) = /var/www/images/1.jpg
	return _root + uri.substr(_location_path.size(), uri.size());
}

/**
 * GET
 */

/*
	Returns an extremelly basic html page with a list of files in the directory
*/
std::string Location::dir_listing_content(const std::string &dir_path)
{
	static const std::string header = "<!DOCTYPE html>\n<html>\n<head>\n<title>Directory listing</title>\n</head>\n<body>\n";
	static const std::string footer = "</body>\n</html>\n";
	static const std::string href_open = "<a href=\"";
	static const std::string href_close = "</a>\n";
	static const std::string br = "<br>\n";
	std::string              ret = "";

	struct dirent	        *entry;
	DIR				        *dir;

	ret += header;

	dir = opendir(dir_path.c_str());
	if (!dir)
		return (ret + "Error opening directory" + footer);
	while (true)
	{
		entry = readdir(dir);
		if (!entry)
			break ;
		if (entry->d_name[0] == '.') // Skip hidden files
			continue ;
		const std::string entry_str = std::string(entry->d_name);

		utils::e_path_type type = utils::what_is_this_path(dir_path + entry_str);
		if (type == utils::FILE)
			ret += "File: ";
		else if (type == utils::DIRECTORY)
			ret += "Directory: ";
		ret += href_open + entry_str + "\">" + entry_str + href_close + br;
	}
	closedir(dir);
	ret += footer;
	return (ret);
}

void   Location::build_response_get_dir(Client &client, std::string &local_path)
{
    if (_indexes.size() > 0)
    {
        for (size_t i = 0; i < _indexes.size(); i++)
        {
            std::string index_path = local_path + _indexes[i];
            if (utils::what_is_this_path(index_path) == utils::FILE)
            {
                logs::SdevLog("Get-Dir: Sending Index");
                client.response.file_path_to_send = index_path;
                client.response.body = "";
                client.response.status_code = 200;
                client.response.headers += "Content-Type: application/octet-stream\r\n";
                return ;
            }
        }
    }

    if (_dir_listing == false) {
        throw Http::HttpException(403);
    }
    logs::SdevLog("Get-Dir: Dir listing");
	client.response.file_path_to_send = "";
	client.response.body = dir_listing_content(local_path);
	client.response.status_code = 200;
	client.response.body_size = client.response.body.size();
	client.response.headers = \
		"Content-Type: text/html\r\n"\
		"Content-Length: " + utils::ito_str(client.response.body_size) + "\r\n";
}

void Location::build_response_get_file(Client &client, std::string &local_path)
{
	client.response.file_path_to_send = local_path;
	client.response.body = "";
	client.response.status_code = 200;
	client.response.headers = "Content-Type: application/octet-stream\r\n";
}

void   Location::handle_get_request(Client &client)
{
    std::string        uri_asked_file = get_local_path(client.request.uri);
    utils::e_path_type type = utils::what_is_this_path(uri_asked_file);

    switch (type)
    {
        case utils::FILE:
            logs::SdevLog("Get-File");
            build_response_get_file(client, uri_asked_file);
            break;
        case utils::DIRECTORY:
        {
            logs::SdevLog("Get-Dir");
            build_response_get_dir(client, uri_asked_file);
            break;
        }
        default:
            logs::SdevLog("Get: Not found: " + uri_asked_file);
            throw Http::HttpException(404);
    }
}

/**
 * POST
 */

void    Location::download_client_file(Client &client, std::string &file_path)
{
    // Many things to change here, this is just a placeholder
    std::ofstream file(file_path.c_str(), std::ios::binary);

    if (!file.is_open())
        throw Http::HttpException(500);
    file << client.request.body;
    file.close();
}

void    Location::post_to_cgi(Client &client, std::string &local_path)
{
    (void)client;
    (void)local_path;
}

void	Location::handle_post_request(Client &client)
{
    (void)client;
    // logs::SdevLog("Post: Downloading file");
    // if (download_client_file)
        // download_client_file(client, get_local_path(client.request.uri));
    // logs::SdevLog("Post: To CGI");
}

/**
 * DELETE
 */

void    Location::delete_file(std::string &file_path)
{
    if (remove(file_path.c_str()) != 0)
        throw Http::HttpException(500);
}

void	Location::handle_delete_request(Client &client)
{
    std::string        local_path = get_local_path(client.request.uri);
    utils::e_path_type type = utils::what_is_this_path(local_path);

    if (type == utils::DIRECTORY) {
        logs::SdevLog("Deleting dir, not allowed");
        throw Http::HttpException(403);
    }
    if (type == utils::INVALID) {
        logs::SdevLog("Not found");
        throw Http::HttpException(404);
    }
    logs::SdevLog("Deleting file");
    delete_file(local_path);
}
