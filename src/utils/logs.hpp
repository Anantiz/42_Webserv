#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "utils.hpp"

// ANSI color codes
#define RED "\033[91m"
#define GREEN "\033[92m"
#define YELLOW "\033[93m"
#define BLUE "\033[94m"
#define MAGENTA "\033[95m"
#define CYAN "\033[96m"
#define RESET "\033[0m"

/*
	** Logs message of different levels into associated output streams.

	Any `server` class instance will have it's own logs instance
		> REASON: Each server instance will have it's own log file

*/
class logs
{
public:
	enum LogLevel
	{
		DEV,
		DEBUG,
		INFO,
		WARNING,
		ERROR
	};
	// For development
	// For release
	// const static logs::LogLevel logs::defaultLogLevel = logs::INFO;

	/*
	 ** Meta
	 */

	~logs();

	logs(void);																							  // Default constructor
	logs(const enum LogLevel log_level);																  // Set log level only
	logs(enum LogLevel log_level, const std::string &errLogFileName, const std::string &infoLogFileName); // Give log files

	/*
	 ** Members
	 */

	// Logs message to the appropriate output stream
	void devLog(const std::string &msg) const;
	void debugLog(const std::string &msg) const;
	void infoLog(const std::string &msg) const;
	void warnLog(const std::string &msg) const;
	void errLog(const std::string &msg) const;

private:
	const static logs::LogLevel defaultLogLevel = logs::DEV;

	enum LogLevel _logLevel;

	// Output streams
	std::ostream *_highPriorityOut; // Errors, warnings
	std::ostream *_lowPriorityOut;	// Info, debug, dev

	// Log files if redirected
	std::ofstream _errLogFile;	// Errors, warnings
	std::ofstream _infoLogFile; // Info, debug, dev

	// Lowest level of logs to be displayed
	// Use warpers instead of calling log directly
	void log(enum LogLevel level, const std::string &msg) const;
};
