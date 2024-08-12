#include "parser.hpp"

/*
███████ ███████ ██████  ██    ██ ███████ ██████
██      ██      ██   ██ ██    ██ ██      ██   ██
███████ █████   ██████  ██    ██ █████   ██████
     ██ ██      ██   ██  ██  ██  ██      ██   ██
███████ ███████ ██   ██   ████   ███████ ██   ██
*/


size_t ConfigParser::listen_directive(const std::string &unit, size_t start, Server &server)
{
    std::string word;
    int err;

    while (true)
    {
        word = utils::read_word(unit, start, start);
        if (word == "" || word == ";")
            break;
        server.add_port(utils::str_to_int(word, err));
        if (err)
            throw std::runtime_error("Invalid port: " + word);
    }
    if (word == "")
        throw std::runtime_error("Unexpected end of file: listen directive");
    if (server.get_ports().empty())
        throw std::runtime_error("No port specified: listen directive");
    return start;
}

size_t ConfigParser::server_name_directive(const std::string &unit, size_t start, Server &server)
{
    std::string word;

    while (true)
    {
        word = utils::read_word(unit, start, start);
        if (word == "" || word == ";")
            break;
        server.add_name(word);
    }
    if (word == "")
        throw std::runtime_error("Unexpected end of file: server_name directive");
    if (server.get_names().empty())
        throw std::runtime_error("No server name specified: server_name directive");
    return start;
}

size_t ConfigParser::error_page_directive(const std::string &unit, size_t start, Server &server)
{
    std::string word;
    int err;

    word = utils::read_word(unit, start, start);
    if (word == "")
        throw std::runtime_error("Unexpected end of file: error_page directive");
    int status_code = utils::str_to_int(word, err);
    if (err)
        throw std::runtime_error("Invalid status code: " + word);
    word = utils::read_path(unit, start, start);
    if (word == "")
        throw std::runtime_error("Unexpected end of file: error_page directive");
    server.add_custom_error_page(status_code, word);
    word = utils::read_word(unit, start, start);
    if (word != ";")
        throw std::runtime_error("Unexpected token: " + word);
    return start;
}

size_t ConfigParser::client_body_size_directive(const std::string &unit, size_t start, Server &server)
{
    std::string word;
    int err;

    word = utils::read_word(unit, start, start);
    if (word == "")
        throw std::runtime_error("Unexpected end of file: client_body_size directive");
    size_t size = utils::str_to_int(word, err);
    if (err)
        throw std::runtime_error("Invalid size: " + word);
    server.set_max_client_body_size(size);
    word = utils::read_word(unit, start, start);
    if (word != ";")
        throw std::runtime_error("Unexpected token: " + word);
    return start;
}


/*
██       ██████   ██████  █████  ████████ ██  ██████  ███    ██
██      ██    ██ ██      ██   ██    ██    ██ ██    ██ ████   ██
██      ██    ██ ██      ███████    ██    ██ ██    ██ ██ ██  ██
██      ██    ██ ██      ██   ██    ██    ██ ██    ██ ██  ██ ██
███████  ██████   ██████ ██   ██    ██    ██  ██████  ██   ████
*/

size_t ConfigParser::root_directive(const std::string &unit, size_t start, Location &location)
{
    std::string word;

    word = utils::read_path(unit, start, start);
    if (word == "")
        throw std::runtime_error("Unexpected end of file: root directive");
    location.set_root(word);
    word = utils::read_word(unit, start, start);
    if (word != ";")
        throw std::runtime_error("Unexpected token: " + word);
    return start;
}

size_t ConfigParser::index_directive(const std::string &unit, size_t start, Location &location)
{
    std::string word;

    while (true)
    {
        word = utils::read_word(unit, start, start);
        if (word == "" || word == ";")
            break;
        location.add_index(word);
    }
    if (word == "")
        throw std::runtime_error("Unexpected end of file: index directive");
    if (location.get_indexes().empty())
        throw std::runtime_error("No index specified: index directive");
    return start;
}

size_t ConfigParser::directory_listing_directive(const std::string &unit, size_t start, Location &location)
{
    std::string word;

    word = utils::read_word(unit, start, start);
    if (word == "")
        throw std::runtime_error("Unexpected end of file: directory_listing directive");
    if (word == "on")
        location.set_dir_listing(true);
    else if (word == "off")
        location.set_dir_listing(false);
    else
        throw std::runtime_error("Directory listing Invalid value: " + word);
    word = utils::read_word(unit, start, start);
    if (word != ";")
        throw std::runtime_error("Unexpected token: " + word);
    return start;
}

size_t ConfigParser::cgi_directive(const std::string &unit, size_t start, Location &location)
{
    std::string word;
    std::pair<std::string, std::string> cgi;

    word = utils::read_path(unit, start, start);
    if (word == "")
        throw std::runtime_error("Unexpected end of file: cgi directive, provide a path and an extension");
    cgi.first = word;
    word = utils::read_word(unit, start, start);
    if (word == "")
        throw std::runtime_error("Unexpected end of file: cgi directive, no extension provided");
    cgi.second = word;
    location.add_cgi(cgi);
    word = utils::read_word(unit, start, start);
    if (word != ";")
        throw std::runtime_error("Unexpected token: " + word);
    return start;
}

size_t ConfigParser::allow_methods_directive(const std::string &unit, size_t start, Location &location)
{
    std::string word;
    int err;

    word = utils::read_word(unit, start, start);
    if (word == "")
        throw std::runtime_error("Unexpected end of file: allow_methods directive");
    int mask = 0;
    for (size_t i = 0; i < word.size(); i++)
    {
        if (word[i] == 'G')
            mask |= 0b001;
        else if (word[i] == 'P')
            mask |= 0b010;
        else if (word[i] == 'D')
            mask |= 0b100;
        else
            throw std::runtime_error("Invalid method: only G,P,D are allowed: " + word[i]);
    }
    location.set_allowed_methods(mask);
    word = utils::read_word(unit, start, start);
    if (word != ";")
        throw std::runtime_error("Unexpected token: " + word);
    return start;
}

size_t ConfigParser::return_directive(const std::string &unit, size_t start, Location &location)
{
    std::string word;
    int err;

    word = utils::read_word(unit, start, start);
    if (word == "")
        throw std::runtime_error("Unexpected end of file: return directive");
    int status_code = utils::str_to_int(word, err);
    if (err)
        throw std::runtime_error("Invalid status code: " + word);
    word = utils::read_path(unit, start, start);
    if (word == "")
        throw std::runtime_error("Unexpected end of file: return directive");
    location.set_redirect(std::make_pair(status_code, word));
    word = utils::read_word(unit, start, start);
    if (word != ";")
        throw std::runtime_error("Unexpected token: " + word);
    return start;
}

size_t ConfigParser::upload_dir_directive(const std::string &unit, size_t start, Location &location)
{
    std::string word;

    word = utils::read_path(unit, start, start);
    if (word == "")
        throw std::runtime_error("Unexpected end of file: upload_dir directive");
    location.set_upload_dir(word);
    word = utils::read_word(unit, start, start);
    if (word != ";")
        throw std::runtime_error("Unexpected token: " + word);
    return start;
}
