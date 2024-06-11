#include "debug.hpp"

// Keep that for development
logs::LogLevel logs::g_log_level = logs::DEBUG;

/*
	** This function logs a message with a specific level.

	** @param level: [DEBUG, INFO, WARNING, ERROR]
*/
void	logs::log(enum logs::LogLevel level, const std::string &msg)
{
	if (level < logs::g_log_level)
		return;
	switch (level)
	{
	case (DEBUG):
		std::cerr << "DEBUG: ";
		break;
	case (INFO):
		std::cerr << BLUE"INFO: ";
		break;
	case (WARNING):
		std::cerr << YELLOW"WARNING: ";
		break;
	case (ERROR):
		std::cerr << RED"ERROR: ";
		break;
	default:
		std::cerr << MAGENTA"What the dog doing ? ";
		return;
	}
	std::cerr << msg << RESET << std::endl;
}