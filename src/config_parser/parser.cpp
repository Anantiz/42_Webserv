#include "parser.hpp"

ConfigParser::ConfigParser(const char *path)
{
    logger.infoLog("Parsing config file: " + std::string(path));
    fd = open(path, O_RDONLY);
    if (fd == -1)
        throw std::runtime_error("Could not open config file");
    buff_size = 16384;
    _fully_read = false;
    init_directives_ptr();
    try {
        utils::bzero(buff, buff_size);
       parse_file();
    } catch (std::exception &e) {
        logger.errLog("Error parsing config file: " + std::string(e.what()));
        throw;
    }
}

ConfigParser::~ConfigParser()
{
}

void ConfigParser::init_directives_ptr( void )
{
    server_keywords["listen"] = ConfigParser::listen_directive;
    server_keywords["server_name"] = ConfigParser::server_name_directive;
    server_keywords["error_page"] = ConfigParser::error_page_directive;
    server_keywords["client_body_size"] = ConfigParser::client_body_size_directive;

    location_keywords["root"] = ConfigParser::root_directive;
    location_keywords["index"] = ConfigParser::index_directive;
    location_keywords["directory_listing"] = ConfigParser::directory_listing_directive;
    location_keywords["cgi_pass"] = ConfigParser::cgi_directive;
    location_keywords["allow_methods"] = ConfigParser::allow_methods_directive;
    location_keywords["return"] = ConfigParser::return_directive;
    location_keywords["upload_dir"] = ConfigParser::upload_dir_directive;
}

std::vector<Server *>	ConfigParser::get_servers( void )
{
    return _servers;
}


std::string ConfigParser::get_next_unit( void )
{
    size_t  buff_i = 0;
    ssize_t read = 0;

    int    bracket_count = 0;
    // Dumbfuck inneficient, but protects from bad input
    while (buff_i < buff_size)
    {
        read = ::read(fd, buff + buff_i, 1);
        if (read < 0)
            throw std::runtime_error("Could not read from config file");
        if (read == 0)
            {_fully_read = true;break;}

        if (buff[buff_i] == '{')
            ++bracket_count;
        else if (buff[buff_i] == '}')
        {
            --bracket_count;
            if (!bracket_count)
                {++buff_i;break;}
        }
        else if (buff[buff_i] == ';')
        {
            if (!bracket_count)
                {++buff_i;break;}
        }
        ++buff_i;
    }

    if (buff_i == buff_size)
        throw std::runtime_error("Config file is too big");
    if (bracket_count)
        throw std::runtime_error("Invalid blocks in config file, unmatched brackets");
    if (!read)
        return std::string("");

    if (!buff_i && buff[buff_i] == ';')
    {
        utils::bzero(buff, buff_i);
        return get_next_unit();
    }
    std::string ret;
    ret =  std::string(buff, buff_i);
    utils::bzero(buff, buff_i);
    return ret;
}

size_t    block_open_bracket(const std::string &unit, size_t start)
{
    if (utils::read_word(unit, start, start) != "{")
        throw std::runtime_error("Missing open bracket");
    return start;
}

size_t ConfigParser::server_block(const std::string &unit, size_t start)
{
    start = block_open_bracket(unit, start);
    Server *serv = new Server();
    while (true)
    {
        std::string sub_directive = utils::read_word(unit, start, start);
        if (sub_directive == "}")
            {logger.devLog("\tEnd of server block");break;}
        logger.devLog("\tDirective: " + sub_directive);
        if (sub_directive == "location")
        {
            try {
                start = location_block(unit, start, serv);
            } catch (std::exception &e)
            {
                delete serv;
                logger.errLog("Error parsing location block: " + std::string(e.what()));
                throw std::runtime_error("Unexpected end of file: location Server unclosed");
            }
        }
        else if (sub_directive == "") {
            delete serv;
            throw std::runtime_error("Unexpected end of file: location Server unclosed");
        }
        else
        {
            std::map<std::string, t_server_directive_ptr>::iterator it = server_keywords.find(sub_directive);
            if (it == server_keywords.end()) {
                delete serv;
                throw std::runtime_error("Invalid directive: " + sub_directive);
            }
            start = it->second(unit, start, *serv);
        }
    }
    try {
        if (serv)
            serv->check_mandatory_directives();
        else
            throw std::runtime_error("We messed up");
    } catch (std::exception &e) {
        delete serv;
        throw;
    }
    _servers.push_back(serv);
    return start;
}

size_t ConfigParser::location_block(const std::string &unit, size_t start, Server *serv)
{
    std::string location_path = utils::read_path(unit, start, start);
    logger.devLog("\t\tLocation path: " + location_path);
    start = block_open_bracket(unit, start);

    Location *location = new Location(location_path);
    while (true)
    {
        std::string sub_directive = utils::read_word(unit, start, start);
        if (sub_directive == "}") {
            logger.devLog("\t\tEnd of Location block");
            break;
        }
        logger.devLog("\t\tDirective: " + sub_directive);

        if (sub_directive == "") {
            delete location;
            throw std::runtime_error("Unexpected end of file: location block unclosed");
        }
        else
        {
            std::map<std::string, t_location_directive_ptr>::iterator it = location_keywords.find(sub_directive);
            if (it == location_keywords.end()) {
                delete location;
                throw std::runtime_error("Invalid directive: " + sub_directive);
            }
            start = it->second(unit, start, *location);
        }
    }
    try {
        if (location)
            location->check_mandatory_directives();
        else
            throw std::runtime_error("Unknown error");
    } catch (std::exception &e) {
        delete location;
        throw;
    }
    serv->add_location(location);
    return start;
}

void ConfigParser::parse_file( void )
{
    try {
        while (!_fully_read)
        {
            std::string unit = get_next_unit();
            if (unit == "")
                {logger.devLog("End of file");break;}
            logger.devLog("Unit: " + unit);
            size_t start = 0;
            std::string directive;

            directive = utils::read_word(unit, start, start);
            logger.devLog("Directive: " + directive);

            if (directive == "server")
            {
                try {
                    server_block(unit, start);
                }catch (std::exception &e) {
                    throw std::runtime_error("Error parsing server block: " + std::string(e.what()));
                }
            }
            else
                throw std::runtime_error("Invalid directive: " + directive);
        }
    } catch (std::exception &e) {
        close(fd);
        for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it)
          delete *it;
        throw;
    }
    close(fd);
}