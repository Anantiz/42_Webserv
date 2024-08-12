#include "parser.hpp"

ConfigParser::ConfigParser(const char *path)
{
    logger.infoLog("Parsing config file: " + std::string(path));
    fd = open(path, O_RDONLY);
    if (fd == -1)
        throw std::runtime_error("Could not open config file");
    buff_size = 16384;
    _fully_read = false;
    try {
        utils::ft_bzero(buff, buff_size);
       parse_file();
    } catch (std::exception &e) {
        logger.errLog("Error parsing config file: " + std::string(e.what()));
        throw;
    }
}

ConfigParser::~ConfigParser()
{
}

std::vector<Server *>	&ConfigParser::get_servers( void )
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
                break;
        }
        else if (buff[buff_i] == ';')
        {
            if (!bracket_count)
                break;
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
        utils::ft_bzero(buff, buff_i);
        return get_next_unit();
    }
    std::string ret;
    ret =  std::string(buff, buff_i);
    utils::ft_bzero(buff, buff_i);
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
    while (true)
    {
        std::string sub_directive = utils::read_word(unit, start, start);
        logger.devLog("\tSub directive: " + sub_directive);
        if (sub_directive == "location")
        {
            try {
                start = location_block(unit, start);
            } catch (std::exception &e)
            {
                logger.errLog("Error parsing location block: " + std::string(e.what()));
                break;
            }
        }
        if (sub_directive == "}")
            {logger.devLog("End of server block");break;}
        else if (sub_directive == "")
            {throw std::runtime_error("Unexpected end of file: location Server unclosed") ;break;}
    }
    return start;
}

size_t ConfigParser::location_block(const std::string &unit, size_t start)
{
    std::string location_path = utils::read_path(unit, start, start);
    start = block_open_bracket(unit, start);
    while (true)
    {
        std::string sub_directive = utils::read_word(unit, start, start);
        logger.devLog("\t\tSub-Sub directive: " + sub_directive);
        if (sub_directive == "}")
            {logger.devLog("End of Location block");break;}
        else if (sub_directive == "")
            {throw std::runtime_error("Unexpected end of file: location block unclosed") ;break;}
    }
    return start;
}

void ConfigParser::parse_file( void )
{
    while (!_fully_read)
    {
        std::string unit = get_next_unit();
        if (unit == "")
            {logger.devLog("End of file");break;}
        size_t start = 0;
        std::string directive;

        directive = utils::read_word(unit, start, start);
        logger.devLog("Directive: " + directive);

        if (directive == "server")
        {
            try {
                server_block(unit, start);
            }catch (std::exception &e) {
                logger.errLog("Error parsing server block: " + std::string(e.what()));
            }
        }
        else
            throw std::runtime_error("Invalid directive: " + directive);
    }
}