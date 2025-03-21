#include "webserv.hpp"

#define DEFAULT_CONFIG_FILE_PATH "configs/australia.conf"

int main(int ac, char **av)
{
	const char *config_path;
	logs		globalLog;
	int			crash_count = 0;

	if (ac > 1)
		config_path = av[1];
	else
		config_path = DEFAULT_CONFIG_FILE_PATH;

	try {
		Cluster cluster(config_path);
		while (crash_count < 3)
		{
			try {
				signal_handler();
				globalLog.infoLog("Starting the cluster, you can gracefully shut it down with [Ctrl+C] or Kill it with [Ctrl+\\]");
				cluster.start();
				return (EXIT_SUCCESS);
			} catch (std::exception &e) {
				globalLog.errLog("A crtical error occured, attempting to restart the cluster: " + utils::anything_to_str(crash_count) + " out of 3\nDetails: " + std::string(e.what()));
			}
			crash_count ++;
		}
		return (EXIT_FAILURE);

	} catch (std::exception &e) {
		globalLog.errLog("Could not start cluster: " + std::string(e.what()));
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
