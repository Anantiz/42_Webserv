#include "logs.hpp"

/*
███    ███ ███████ ████████  █████
████  ████ ██         ██    ██   ██
██ ████ ██ █████      ██    ███████
██  ██  ██ ██         ██    ██   ██
██      ██ ███████    ██    ██   ██
*/

/*
 ** Constructors
 */

/*
	Default:
		> all logs are printed to std::cerr
		> log level is set to defaultLogLevel
*/
logs::logs() :  _logLevel(logs::defaultLogLevel)
{
}

/*
	Set log level only:
		> all logs are printed to std::cerr
		> log level is set to log_level
*/
logs::logs(const enum logs::LogLevel log_level) : _logLevel(log_level)
{
}

logs::~logs()
{
}

/*
███████ ██    ██ ███    ██  ██████ ████████ ██  ██████  ███    ██ ███████
██      ██    ██ ████   ██ ██         ██    ██ ██    ██ ████   ██ ██
█████   ██    ██ ██ ██  ██ ██         ██    ██ ██    ██ ██ ██  ██ ███████
██      ██    ██ ██  ██ ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
██       ██████  ██   ████  ██████    ██    ██  ██████  ██   ████ ███████
*/

void logs::SdevLog(const std::string &msg)
{
	std::cerr << WHITE << "[DEV] " << RESET << msg << std::endl;
}


/*
	** This function logs a message with a specific level.

	** @param level: [DEV, DEBUG, INFO, WARNING, ERROR]
*/
void logs::log(enum logs::LogLevel level, const std::string &msg) const
{
	if (level < logs::_logLevel)
		return;
	switch (level)
	{
	case (DEV):
		std::cerr << WHITE << "[DEV] " << RESET << msg << std::endl;
		break;
	case (DEBUG):
		std::cerr << WHITE << "[DEBUG] " << RESET << msg << std::endl;
		break;
	case (INFO):
		std::cerr  << BLUE "[INFO] " << RESET << msg << std::endl;
		break;
	case (WARNING):
		std::cerr  << YELLOW "[WARNING] " << RESET << msg << std::endl;
		break;
	case (ERROR):
		std::cerr  << RED "[ERROR] " << RESET << msg << std::endl;
		break;
	default:
		std::cerr << MAGENTA "What the dog doing ? " << RESET << msg << std::endl;
		return;
	}
}

/*
 ** These functions are shortcuts for log(LEVEL, msg)
 */

void logs::devLog(const std::string &msg) const
{
	log(DEV, msg);
}

void logs::debugLog(const std::string &msg) const
{
	log(DEBUG, msg);
}

void logs::infoLog(const std::string &msg) const
{
	log(INFO, msg);
}

void logs::warnLog(const std::string &msg) const
{
	log(WARNING, msg);
}

void logs::errLog(const std::string &msg) const
{
	log(ERROR, msg);
}
