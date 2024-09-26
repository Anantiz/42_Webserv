#include "server.hpp"

Server::Server()
{
    _max_client_body_size = -1;
    _was_set_max_client_body_size = false;
	_was_set_protocol = false;
}

Server::~Server()
{
    for (size_t i=0; i < _locations.size(); i++)
        delete _locations[i];
}

void    Server::check_mandatory_directives( void )
{
    if (_ports.size() == 0)
        throw std::runtime_error("No port specified for server, cannot start server without a port");
    if (_locations.size() == 0)
        throw std::runtime_error("No location specified for server, have at least one location");
    if (_names.size() == 0) {
        _logger.warnLog("No server-name specified, will use the default server-name");
        _names.push_back("default_server");
    }
}

void Server::add_port(u_int16_t p)
{
    if (std::find(_ports.begin(), _ports.end(), p) != _ports.end())
        _logger.warnLog("Ignoring duplicate server-port");
    else
        _ports.push_back(p);
}

void Server::add_name(const std::string &n)
{
    std::string name = utils::as_lower(n);
    if (std::find(_names.begin(), _names.end(), name) != _names.end())
        _logger.warnLog("Ignoring duplicate server-name: " + name);
    else
        _names.push_back(name);
}

void Server::set_protocol(Http::e_protocol p)
{
    if (_was_set_protocol)
        throw std::runtime_error("Duplicate directive: protocol");
    _was_set_protocol = true;
    _protocols = p;
}

void Server::add_location(Location *l)
{
    // 1. Check duplicates
    // 2. Sort by path length

    for (std::vector<Location *>::iterator it = _locations.begin(); it != _locations.end(); it++)
    {
        if ((*it)->get_location_path() == l->get_location_path())
            throw std::runtime_error("Duplicate location: " + l->get_location_path());
    }
    for (std::vector<Location *>::iterator it = _locations.begin(); it != _locations.end(); it++)
    {
        if ((*it)->get_location_path().size() > l->get_location_path().size())
        {
            _locations.insert(it, l);
            return ;
        }
    }
    _locations.push_back(l);
}

void Server::add_custom_error_page(int status_code, std::string path)
{
    // Check valid code
    static const int valid_codes[] = {\
    400, 401, 402, 403, 404, 405, 406, 407, 408, 409, \
    410, 411, 412, 413, 414, 415, 416, 417, 418, 421, \
    422, 423, 424, 425, 426, 428, 429, 431, 451, 500, \
    501, 502, 503, 504, 505, 506, 507, 508, 510, 511};
    int i;
    for (i = 0; i < 40; i++)
        if (status_code == valid_codes[i])
            break;
    if (i == 40)
        throw std::runtime_error("Invalid status code for custom error page");
    if (_custom_error_pages.find(status_code) != _custom_error_pages.end())
        throw std::runtime_error("Duplicate directive: 2 error pages for the same status code");
    if (access(path.c_str(), R_OK) == -1)
        throw std::runtime_error("Invalid path for custom error page");
    _custom_error_pages[status_code] = path;
}

void Server::set_max_client_body_size(size_t s)
{
    if (_was_set_max_client_body_size)
        throw std::runtime_error("Duplicate directive: client_body_size");
    if (_max_client_body_size > 1000000000)
        _logger.warnLog("Client body size is larger than 1GB, are you sure ? The server might get overhelmed by only a few clients");
    _was_set_max_client_body_size = true;
    _max_client_body_size = s;
}

std::vector<u_int16_t> &Server::get_ports() { return _ports; }

std::vector<std::string> &Server::get_names() { return _names; }

Http::e_protocol &Server::get_protocol() { return _protocols; }

std::map<int, std::string> &Server::get_custom_error_pages() { return _custom_error_pages; }

size_t &Server::get_max_client_body_size() { return _max_client_body_size; }

/*
██████  ██    ██ ██████  ██      ██  ██████
██   ██ ██    ██ ██   ██ ██      ██ ██
██████  ██    ██ ██████  ██      ██ ██
██      ██    ██ ██   ██ ██      ██ ██
██       ██████  ██████  ███████ ██  ██████
*/

void Server::handle_client_request(Client &client)
{
    // Keep it as this for now,
    // Later, allow processing even if not all data is received
    if (client.connection_status < Client::BODY_ALL_RECEIVED)
        return ;

    _logger.devLog("Handling client request on server:" + client.server->get_names()[0]);
    if (client.request.uri.empty()) // Cuz parsing just discard it if it's empty
        client.request.uri = "/";
    try {
        Location& l = match_best_location(client.request.uri);
        _logger.devLog("Matched location rooted at: " + l.get_root());
        l.build_request_response(client);
    } catch (Http::HttpException &e) {
        _logger.devLog("Server Caught HttpException: " + utils::anything_to_str(e.get_status_code()));
        build_error_response(client, e.get_status_code());
        return ;
    } catch (std::exception &e) {
        _logger.warnLog("Server Caught exception: " + std::string(e.what()));
        build_error_response(client, 500);
        return ;
    }
}

/*
██████  ██████  ██ ██    ██  █████  ████████ ███████
██   ██ ██   ██ ██ ██    ██ ██   ██    ██    ██
██████  ██████  ██ ██    ██ ███████    ██    █████
██      ██   ██ ██  ██  ██  ██   ██    ██    ██
██      ██   ██ ██   ████   ██   ██    ██    ███████
*/

void Server::build_error_response(Client &client, int status_code) const
{
    client.response.status_code = status_code;
    std::map<int, std::string>::const_iterator cutom_error_page = _custom_error_pages.find(status_code);
    if (cutom_error_page == _custom_error_pages.end())
        client.generate_quick_response("");
    else
        client.generate_quick_response(cutom_error_page->second);
}

Location &Server::match_best_location(const std::string &uri) const
{
    Location *best_location = NULL;
    size_t best_score = 0;

    logs::SdevLog("Matching location for uri: " + uri);
    for (std::vector<Location *>::const_iterator it = _locations.begin(); it != _locations.end(); it++)
    {
        size_t score = (*it)->count_blocks(uri);
        logs::SdevLog("Location Match Score with: " + (*it)->get_location_path() + " is " + utils::anything_to_str(score));
        if (score > best_score)
        {
            best_score = score;
            best_location = *it;
        }
    }
    if (best_location == NULL)
        throw Http::HttpException(404);
    return *best_location;
}
