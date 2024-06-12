#pragma once
// This file contains the general includes that are used in the project
// as well as pre-declarations of custom classes and functions
#include "webserv.hpp"

/* Config file directives */

class config;
class configServer;
class configLocation;

/* Run-time classes */

class clientConnection;
class serverConnection;

/* Utils */

class logs;
class regexParser;

/* Exceptions */

class webservException;
