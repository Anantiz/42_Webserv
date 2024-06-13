#pragma once
// This file contains the general includes that are used in the project
// as well as pre-declarations of custom classes and functions
#include "webserv.hpp"

class route
{
public:

	route(/* args */);
	~route();

private:

	/*
		Any optional field that is required to function properly
		is set to the server's value if not provided
	*/

	const std::string _routeUri; // The uri that the route is listening to, and tries to match /* Mandatory */

	const std::string _root; // Default is an empty string /* Optional */
	const std::string _alias; // Default is an empty string /* Optional */
		/*
			First replace the uri with the alias, then prepend the root to the result.
		*/
	const std::vector<std::string> _indexes; // Index.html, index.php ... So a vector of strings /* Optional */

	// Methods
	bool	_allowPOST;       // Default is true
	bool	_allowGET;        // Default is true
	bool	_allowDirListing; // Default is false /* Optional */

};
