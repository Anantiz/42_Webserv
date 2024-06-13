#pragma once
// This file contains the general includes that are used in the project
// as well as pre-declarations of custom classes and functions
#include "webserv.hpp"

/* Config file directives */

class config; // Used for parsing the config file

// Server directives
class server;
class location;

// Client directives
class client;
class clientManager;
class clientConnection;

// Utils
class logs;
class webservException;
