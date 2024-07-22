#include "utils.hpp"

const std::string	utils::ito_str(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

/// @brief Returns whether the path is a FILE, DIRECTORY, or is INVALID
enum utils::e_path_type	utils::what_is_this_path(char *path)
{
	struct stat path_stat;
	
	if (!path || ::stat(path, &path_stat))
		return INVALID;
	if (S_ISDIR(path_stat.st_mode))
		return DIRECTORY;
	if (S_ISREG(path_stat.st_mode))
		return FILE;
	return INVALID;
}