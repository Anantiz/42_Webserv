#pragma once

#include <iostream>
#include <string>

#define RED "\033[91m"
#define GREEN "\033[92m"
#define YELLOW "\033[93m"
#define BLUE "\033[94m"
#define MAGENTA "\033[95m"
#define CYAN "\033[96m"
#define RESET "\033[0m"


namespace logs
{
	enum LogLevel
	{
		DEBUG,
		INFO,
		WARNING,
		ERROR
	};

	// Set by the config file
	extern enum LogLevel g_log_level;

	void	log(enum LogLevel level, const std::string &msg);

}

// #define L_DEBUG LogLevel::DEBUG
// #define L_INFO LogLevel::INFO
// #define L_WARNING LogLevel::WARNING
// #define L_ERROR LogLevel::ERROR