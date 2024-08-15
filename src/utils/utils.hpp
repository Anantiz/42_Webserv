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

class Server;

namespace utils
{
	enum e_path_type {
		INVALID,
		DIRECTORY,
		FILE,
	};

	const std::string		ito_str(int i);
	template <typename T>
	const std::string		anything_to_str(T i);
	enum e_path_type		what_is_this_path(const std::string &path);
	int						str_to_int(const std::string &str, int &error);

	bool	in_ports(u_int16_t p, std::vector<u_int16_t> &ports);
	bool    in_servers(std::string &name, std::vector<std::pair<std::string, Server *> > &servers);
	bool    ft_isspace(char c);
	void	bzero(void *s, size_t n);

	size_t 		skip_spaces(const std::string &str, size_t start);
	size_t 		skip_comments(const std::string &str, size_t start);
	std::string	read_word(const std::string &str, size_t start, size_t &next_start);
	std::string	read_path(const std::string &str, size_t start, size_t &next_start);
	std::string strip(const std::string &str);


} // namespace utils

#endif // UTILS_HPP