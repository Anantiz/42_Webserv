#pragma once

#include <iostream>
#include <fstream>
#include <string>

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
	const static logs::LogLevel logs::defaultLogLevel = logs::DEV;
	// For release
	// const static logs::LogLevel logs::defaultLogLevel = logs::INFO;

	/* Meta */
	logs( void );
	~logs();

	// Logs message to the appropriate output stream
	void	log(enum LogLevel level, const std::string &msg);

	// Setters
	void	setLogLevel(enum LogLevel level) { _log_level = level; }
	void	setDevOut(std::ostream &devOut) { _devOut.rdbuf(devOut.rdbuf()); }
	void	setDebugOut(std::ostream &debugOut) { _debugOut.rdbuf(debugOut.rdbuf()); }
	void	setInfoOut(std::ostream &infoOut) { _infoOut.rdbuf(infoOut.rdbuf()); }
	void	setWarningOut(std::ostream &warningOut) { _warningOut.rdbuf(warningOut.rdbuf()); }
	void	setErrorOut(std::ostream &errorOut) { _errorOut.rdbuf(errorOut.rdbuf()); }
	// Getters (Not needed)
	// Blank

private:

	// Output streams
	std::ostream *_devOut;
	std::ostream *_debugOut;
	std::ostream *_infoOut;
	std::ostream *_warningOut;
	std::ostream *_errorOut;

	enum LogLevel _log_level;
};