#include "server.hpp"

Server::Server()
{
    _max_client_body_size = -1;
    _was_set_max_client_body_size = false;
	_was_set_protocol = false;
}

Server::~Server()
{
    for (std::vector<Location *>::iterator it = _locations.begin(); it != _locations.end(); it++)
        delete *it;
}

void    Server::check_mandatory_directives( void )
{
    if (_ports.size() == 0)
        throw std::runtime_error("No port specified for server, cannot start server without a port");
    if (_locations.size() == 0)
        throw std::runtime_error("No location specified for server, have at least one location");
}

void Server::add_port(u_int16_t p)
{
    if (p > 65535)
        throw std::runtime_error("Invalid port number");
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
    _locations.push_back(l);
}

void Server::add_custom_error_page(int status_code, std::string path)
{
    // Check valid code
    static const int const valid_codes[] = {\
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

std::vector<u_int16_t> &Server::get_ports()
{
    return _ports;
}

std::vector<std::string> &Server::get_names()
{
    return _names;
}

Http::e_protocol &Server::get_protocol()
{
    return _protocols;
}

std::map<int, std::string> &Server::get_custom_error_pages()
{
    return _custom_error_pages;
}

size_t &Server::get_max_client_body_size()
{
    return _max_client_body_size;
}

/*
██████  ██    ██ ██████  ██      ██  ██████
██   ██ ██    ██ ██   ██ ██      ██ ██
██████  ██    ██ ██████  ██      ██ ██
██      ██    ██ ██   ██ ██      ██ ██
██       ██████  ██████  ███████ ██  ██████
*/

void Server::handle_client_request(Client &client)
{
    try {
        match_best_location(client.request.uri).build_request_response(client);
    } catch (Http::HttpException &e) {
        build_error_response(client, e.get_status_code());
        return ;
    } catch (std::exception &e) {
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

void Server::build_error_response(Client &client, int status_code)
{
    std::map<int, std::string>::iterator cutom_error_page = _custom_error_pages.find(status_code);
    if (cutom_error_page != _custom_error_pages.end())
        client.response.file_path_to_send = cutom_error_page->second;
    else
        client.response.file_path_to_send = DEFAULT_ERROR_PAGES_RELATIVE_TO_SERVER + utils::ito_str(status_code) + ".html";
	client.response.body = "";
	client.response.status_code = status_code;
	client.response.headers = "Content-Type: text/html\r\n";
}

Location &Server::match_best_location(std::string &uri)
{
    Location *best_location = *_locations.end();
    size_t best_score = 0;

    for (std::vector<Location *>::iterator it = _locations.begin(); it != _locations.end(); it++)
    {
        size_t score = (*it)->count_blocks(uri);
        if (score > best_score)
        {
            best_score = score;
            best_location = *it;
        }
    }
    if (best_location == *_locations.end())
        throw Http::HttpException(404);
    return *best_location;
}
