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
logs::logs() :  _logLevel(logs::defaultLogLevel), _highPriorityOut(&std::cerr), _lowPriorityOut(&std::cerr)
{
}

/*
	Set log level only:
		> all logs are printed to std::cerr
		> log level is set to log_level
*/
logs::logs(const enum logs::LogLevel log_level) : _logLevel(log_level), _highPriorityOut(&std::cerr), _lowPriorityOut(&std::cerr)
{
}

/*
	Set log level and log files:
		> all logs are printed to std::cerr
		> log level is set to log_level
		> log files are set to errLogFileName and infoLogFileName
*/
logs::logs(enum logs::LogLevel log_level, const std::string &errLogFileName, const std::string &infoLogFileName) : _logLevel(log_level), _highPriorityOut(&std::cerr), _lowPriorityOut(&std::cerr)
{
	_errLogFile.open(errLogFileName.c_str());
	_infoLogFile.open(infoLogFileName.c_str());
	if (_errLogFile.is_open())
		_highPriorityOut = &_errLogFile;
	else
		warnLog("Could not open error log file, redirecting to standard error");
	if (_infoLogFile.is_open())
		_lowPriorityOut = &_infoLogFile;
	else
		warnLog("Could not open info log file, redirecting to standard error");
}

/*
 ** Destructor
 */

logs::~logs()
{
	if (_errLogFile.is_open())
		_errLogFile.close();
	if (_infoLogFile.is_open())
		_infoLogFile.close();
}

/*
███████ ██    ██ ███    ██  ██████ ████████ ██  ██████  ███    ██ ███████
██      ██    ██ ████   ██ ██         ██    ██ ██    ██ ████   ██ ██
█████   ██    ██ ██ ██  ██ ██         ██    ██ ██    ██ ██ ██  ██ ███████
██      ██    ██ ██  ██ ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
██       ██████  ██   ████  ██████    ██    ██  ██████  ██   ████ ███████
*/

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
		*_lowPriorityOut << "[DEV] " << RESET << msg << std::endl;
		break;
	case (DEBUG):
		*_lowPriorityOut << "[DEBUG] " << RESET << msg << std::endl;
		break;
	case (INFO):
		*_highPriorityOut << BLUE "[INFO] " << RESET << msg << std::endl;
		break;
	case (WARNING):
		*_highPriorityOut << YELLOW "[WARNING] " << RESET << msg << std::endl;
		break;
	case (ERROR):
		*_highPriorityOut << RED "[ERROR] " << RESET << msg << std::endl;
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
