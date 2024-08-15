#include "location.hpp"

Location::Location()
{
    _root = "";
    _dir_listing = false;
    _accept_upload = false;
    _allowed_methods = 0b011;
}

Location::Location(std::string &location_path) : _root(location_path)
{
    _dir_listing = false;
    _accept_upload = false;
    _allowed_methods = 0b011;
}

Location::~Location()
{

}

void Location::set_dir_listing(bool b)
{
    _dir_listing = b;
}

void Location::set_accept_upload(bool b)
{
    _accept_upload = b;
}

void Location::set_allowed_methods(int m)
{
    _allowed_methods = m;
}

void Location::add_index(std::string i)
{
    _indexes.push_back(i);
}

void Location::set_root(std::string r)
{
    _root = r;
}

void Location::set_upload_dir(std::string u)
{
    _upload_dir = u;
}

void Location::set_redirect(std::pair<int, std::string> r)
{
    _redirect = r;
}

void Location::add_cgi(std::pair<std::string, std::string> c)
{
    _cgis.push_back(c);
}

void Location::remove_cgi(std::string c)
{
    for (size_t i = 0; i < _cgis.size(); i++)
    {
        if (_cgis[i].first == c)
        {
            _cgis.erase(_cgis.begin() + i);
            return;
        }
    }
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
