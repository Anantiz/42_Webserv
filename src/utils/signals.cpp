#include <signal.h>
#include <stdlib.h>
#include "cluster/cluster.hpp"

static void	signal_handler_sigint(int signum) {
	(void)signum;
	write(1, "\n", 1);
	Cluster::down();
}

void	signal_handler(void)
{
	signal(SIGINT, signal_handler_sigint);
	signal(SIGQUIT, signal_handler_sigint);
}