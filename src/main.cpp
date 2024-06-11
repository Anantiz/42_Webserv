#include "webserv.hpp"

int main(int ac, char **av, char **envp)
{
	(void)envp;

	logs::g_log_level = logs::INFO;

	logs::log(logs::DEBUG, "TEST");
	logs::log(logs::INFO, "TEST");
	logs::log(logs::WARNING, "TEST");
	logs::log(logs::ERROR, "TEST");

	if (ac != 2)
	{
		// logs::log(logs::WARNING, "Usage: ./webserv [config_file]");
	}

	return (EXIT_SUCCESS);
}