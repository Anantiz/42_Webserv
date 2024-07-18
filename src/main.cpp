#include "utils/logs.hpp"
#include <stdlib.h>

int main(int ac, char **av, char **envp)
{
	(void)envp;
	(void)av;

	logs globalLog;

	// if (ac != 2)
	// {
	// 	globalLog.warnLog("No configuration file provided, using default configuration");
	// }

	return (EXIT_SUCCESS);
}