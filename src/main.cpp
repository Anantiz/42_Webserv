#include "utils/logs.hpp"
#include "cluster/cluster.hpp"
#include <stdlib.h>

// I am not doing a full header file just for one stray function
void	signal_handler();

int main(int ac, char **av, char **envp)
{
	(void)envp;
	(void)av;

	logs globalLog;

	if (ac != 2)
		globalLog.warnLog("No configuration file provided, using default configuration");

	try {
		Cluster	cluster(av[1]);
		signal_handler(); // Graceful shutdown
		globalLog.infoLog("Starting the server, you can gracefully shutdown with [Ctrl+C] or [Ctrl+\\]");
		return (cluster.start());
	} catch (std::exception &e) {
		globalLog.errLog(e.what());
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}