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

void Location::add_index(std::string i)
{
    _indexes.push_back(i);
}

void Location::set_root(std::string r)
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

void Location::set_upload_dir(std::string u)
{
    if (_was_set_upload_dir)
        throw std::runtime_error("Duplicate directive: upload_dir");
    _was_set_upload_dir = true;
    _upload_dir = u;
}

void Location::set_redirect(std::pair<int, std::string> r)
{
    if (_was_set_redirect)
        throw std::runtime_error("Duplicate directive: return");
    _was_set_redirect = true;
    _redirect = r;
}

void Location::add_cgi(std::pair<std::string, std::string> c)
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

size_t  Location::count_blocks(std::string &uri)
{
    size_t  count = 0;
    size_t  i = 0;

    while (i < uri.size())
    {
        if (uri[i] == '/')
            count++;
        i++;
    }
    return count;
}

void   Location::build_request_response(Client &client)
{
    switch (client.request.method)
    {
        case Http::GET:
            handle_get_request(client);
            break;
        case Http::POST:
            handle_post_request(client);
            break;
        case Http::DELETE:
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
	// _location_path = /image/chats
	// _root          = /var/www/images
	// uri            = /image/chats/1.jpg
	// local_path     = root + (uri - location_path) = /var/www/images/1.jpg
	return _root + uri.substr(_location_path.size(), uri.size());
}

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
    std::string        local_path = get_local_path(client.request.uri);
    utils::e_path_type type = utils::what_is_this_path(local_path);

    switch (type)
    {
        case utils::FILE:
            build_response_get_file(client, local_path);
            break;
        case utils::DIRECTORY:
        {
            if (_dir_listing) {
                build_response_get_dir(client, local_path);
                break;
            }
        }
        default:
            throw Http::HttpException(404);
    }
}

void	Location::handle_post_request(Client &client)
{
    (void)client;
}

void	Location::handle_delete_request(Client &client)
{
    (void)client;
}