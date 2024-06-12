#include "logs.hpp"

// Keep that for development

// Uncomment this line for production
// logs::LogLevel logs::log_level = logs::INFO;

// Output streams for each logs levels
// Default is std::cerr

/*
███    ███ ███████ ████████  █████
████  ████ ██         ██    ██   ██
██ ████ ██ █████      ██    ███████
██  ██  ██ ██         ██    ██   ██
██      ██ ███████    ██    ██   ██w
*/

logs::logs()
	: _log_level(logs::defaultLogLevel),
	  _devOut(std::cerr.rdbuf()),
	  _debugOut(std::cerr.rdbuf()),
	  _infoOut(std::cerr.rdbuf()),
	  _warningOut(std::cerr.rdbuf()),
	  _errorOut(std::cerr.rdbuf())
{
	// Nothing to do here
}

logs::~logs()
{
	// Close the output streams if they are not std::cerr (so a file stream)
	if (_devOut.rdbuf() != std::cerr.rdbuf())
		static_cast<std::ofstream>(_devOut).close();
	// [...]
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
void logs::log(enum logs::LogLevel level, const std::string &msg)
{

	if (level < logs::defaultLogLevel)
		return;
	switch (level)
	{
	case (DEV):
		_devOut << "DEV: " << msg << RESET << std::endl;
		break;
	case (DEBUG):
		_debugOut << "DEBUG: " << msg << RESET << std::endl;
		break;
	case (INFO):
		_infoOut << BLUE "INFO: " << msg << RESET << std::endl;
		break;
	case (WARNING):
		_warningOut << YELLOW "WARNING: " << msg << RESET << std::endl;
		break;
	case (ERROR):
		_errorOut << RED "ERROR: " << msg << RESET << std::endl;
		break;
	default:
		std::cerr << MAGENTA "What the dog doing ? " << msg << RESET << std::endl;
		return;
	}
}
