#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace utils
{
	enum e_path_type {
		INVALID,
		DIRECTORY,
		FILE,
	};

	const std::string		ito_str(int i);
	enum e_path_type		what_is_this_path(char *path);
} // namespace utils

#endif // UTILS_HPP