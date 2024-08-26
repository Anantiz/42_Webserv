#include "webserv.hpp"

#define DEFAULT_CONFIG_FILE_PATH "configs/webserv-default.conf"
// #define DEFAULT_CONFIG_FILE_PATH "configs/webserv-test-tokens.conf"

int main(int ac, char **av)
{
	const char *config_path;
	logs		globalLog;
	bool		crashed = false;

	if (ac > 1)
		config_path = av[1];
	else
		config_path = DEFAULT_CONFIG_FILE_PATH;

	try {
		Cluster cluster(config_path);
		signal_handler(); // Graceful shutdown
		globalLog.infoLog("Starting the server, you can gracefully shut it down with [Ctrl+C] or [Ctrl+\\]");

		while (cluster.restart_attempt < 2)
		{
			try {
				return cluster.start();
			}
			catch (std::exception &e)
			{
				crashed = true;
				cluster.restart_attempt++;
				globalLog.errLog(e.what());
				globalLog.errLog("A crtical error occured, attempting to restart the server: Attempt " \
				+ utils::ito_str(cluster.restart_attempt) + " of 2");
				cluster.down();
			}
		}
	} catch (std::exception &e) {
		globalLog.errLog(e.what());
		if (crashed)
			globalLog.errLog("Fatal error, issue could not be resolved, shutting down the server");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
