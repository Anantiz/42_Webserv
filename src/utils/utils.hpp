#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <map>

class Server;

namespace utils
{
	enum e_path_type {
		INVALID,
		DIRECTORY,
		FILE,
	};

	template <typename T>
	const std::string anything_to_str(T i)
	{
		std::stringstream ss;
		ss << i;
		return ss.str();
	}

	const std::string		ito_str(int i);
	int						str_to_int(const std::string &str, int &error);


	enum e_path_type		what_is_this_path(const std::string &path);
	bool    				ft_isspace(char c);
	bool					in_ports(u_int16_t p, std::vector<u_int16_t> &ports);
	bool    				in_servers(std::string &name, std::vector<std::pair<std::string, Server *> > &servers);
	void					bzero(void *s, size_t n);

	size_t 					skip_spaces(const std::string &str, size_t start);
	size_t 					skip_comments(const std::string &str, size_t start);
	std::string				read_word(const std::string &str, size_t start, size_t &next_start);
	std::string				read_path(const std::string &str, size_t start, size_t &next_start);
	std::string 			strip(const std::string &str);

	char					**map_to_envp(const std::map<std::string, std::string> &map);
	std::string 			as_lower(const std::string &str);
	const std::string		build_error_page(int error);

	const std::string		get_file_length_header(const std::string& file_path);

} // namespace utils

#endif // UTILS_HPP