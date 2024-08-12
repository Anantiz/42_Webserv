#include "server.hpp"

Server::Server()
{
    _max_client_body_size = -1;
}

Server::~Server()
{
}

void Server::add_port(u_int16_t p)
{
    _ports.push_back(p);
}

void Server::add_name(std::string n)
{
    _names.push_back(n);
}

void Server::set_protocol(Http::e_protocol p)
{
    _protocols = p;
}

void Server::add_location(Location l)
{
    _locations.push_back(l);
}

void Server::add_custom_error_page(int status_code, std::string path)
{
    _custom_error_pages[status_code] = path;
}

void Server::set_max_client_body_size(size_t s)
{
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

std::vector<Location> &Server::get_locations()
{
    return _locations;
}

std::map<int, std::string> &Server::get_custom_error_pages()
{
    return _custom_error_pages;
}

size_t &Server::get_max_client_body_size()
{
    return _max_client_body_size;
}
