#include "webserv.hpp"

#define DEFAULT_CONFIG_FILE_PATH "configs/webserv-default.conf"
// #define DEFAULT_CONFIG_FILE_PATH "configs/webserv-test-tokens.conf"

int main(int ac, char **av)
{
	logs globalLog;
	const char *config_path;

	if (ac > 1)
		config_path = av[1];
	else
		config_path = DEFAULT_CONFIG_FILE_PATH;

	try {
		Cluster cluster(config_path);
		signal_handler(); // Graceful shutdown
		globalLog.infoLog("Starting the server, you can gracefully shut it down with [Ctrl+C] or [Ctrl+\\]");
		return (cluster.start());
	} catch (std::exception &e) {
		globalLog.errLog(e.what());
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}
