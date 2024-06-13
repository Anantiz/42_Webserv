#pragma once

// C++ headers
#include <string>
#include <iomanip>
#include <cstdlib>
#include <exception>

#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <iterator>
#include <vector>
#include <map>

// C headers
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <poll.h>
#include <netdb.h>
#include <dirent.h>
#include <arpa/inet.h>

/*
	** Custom headers
*/

// Every class prototypes of the project
#include "prototypes.hpp"


// Server related
#include "server.hpp"
#include "location.hpp"

// Client related
#include "clientManager.hpp"
#include "clientConnection.hpp"
#include "client.hpp"

// Utils
#include "logs.hpp"
#include "webservException.hpp"