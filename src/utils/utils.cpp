#include "utils.hpp"

const std::string	utils::ito_str(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

template <typename T>
const std::string utils::anything_to_str(T i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

int utils::str_to_int(const std::string &str, int &error)
{
	std::string str2 = str.substr(0, str.find_first_of(" \t\n\r"));
	error = 0;
	if (str2.empty())
	{
		error = 1;
		return 0;
	}

	if (str2.find_first_not_of("0123456789") != std::string::npos)
	{
		error = 2;
		return 0;
	}
	std::stringstream ss(str2);
	int i;
	ss >> i;
	return i;
}


/// @brief Returns whether the path is a FILE, DIRECTORY, or is INVALID
enum utils::e_path_type	utils::what_is_this_path(const std::string &path)
{
	struct stat path_stat;

	if (::stat(path.c_str(), &path_stat))
		return INVALID;
	if (S_ISDIR(path_stat.st_mode))
		return DIRECTORY;
	if (S_ISREG(path_stat.st_mode))
		return FILE;
	return INVALID;
}

bool	utils::in_ports(u_int16_t p, std::vector<u_int16_t> &ports)
{
	for (size_t i=0; i< ports.size(); i++)
		if (ports[i] == p)
			return true;
	return false;
}

bool    utils::in_servers(std::string &name, std::vector<std::pair<std::string, Server *> > &servers)
{
	for (size_t i=0; i< servers.size(); i++)
		if (servers[i].first == name)
			return true;
	return false;
}

bool     utils::ft_isspace(char c)
{
    return (c == ' ' || (c >= 9 && c <= 13));
}

void utils::bzero(void *s, size_t n)
{
	if (!s || !n)
		return;
	char *p = (char *)s;
	while (n--)
		*p++ = 0;
}

std::string utils::strip(const std::string &str)
{
	size_t start = 0;
	size_t end = str.size();
	while (start < end && utils::ft_isspace(str[start]))
		++start;
	while (end > start && utils::ft_isspace(str[end - 1]))
		--end;
	return str.substr(start, end - start);
}

size_t utils::skip_comments(const std::string &str, size_t start)
{
	if (str[start] == '#')
	{
		while (start < str.size() && str[start] != '\n')
			++start;
		start = utils::skip_spaces(str, start);
	}
	if (str[start] == '#')
		return utils::skip_comments(str, start);
	return start;
}

size_t utils::skip_spaces(const std::string &str, size_t start)
{
	while (start < str.size() && utils::ft_isspace(str[start]))
		++start;
	return start;
}

std::string	utils::read_word(const std::string &str, size_t start, size_t &next_start)
{
	start = utils::skip_spaces(str, start);
	start = utils::skip_comments(str, start);
	if (start < str.size())
	{
		next_start = start + 1;
		if (str[start] == '}')
			return "}";
		if (str[start] == '{')
			return "{";
		if (str[start] == ';')
			return ";";
	}
	size_t end = start;
	while (end < str.size() && !utils::ft_isspace(str[end]) \
		&& str[end] != ';' \
		&& str[end] != '{' \
		&& str[end] != '}'\
		&& str[end] != '#')
		{++end;}
	next_start = end;
	return str.substr(start, end - start);
}

std::string	utils::read_path(const std::string &str, size_t start, size_t &next_start)
{
	start = utils::skip_spaces(str, start);
	start = utils::skip_comments(str, start);
	char	q = 0;
	if (str[start] && (str[start] == '"' || str[start] == '\''))
		q = str[start++];

	size_t				end = start;
	bool				escaped = false;
	std::vector<size_t> escaped_indexes;
	if (q) // Handle escaped quotes
	{
		while (str[end])
		{
			if (!escaped && str[end] == '\\')
			{
				escaped_indexes.push_back(end);
				escaped = true;
				++end;
				continue;
			}
			if (!escaped && str[end] == q)
				break;
			escaped = false;
			++end;
		}
	}
	else // Handle escaped whitespaces
	{
		while (str[end])
		{
			if (!escaped && str[end] == '\\')
			{
				escaped_indexes.push_back(end);
				escaped = true;
				++end;
				continue;
			}
			if (!escaped && (utils::ft_isspace(str[end]) || str[end] == ';' || str[end] == '{' || str[end] == '}'))
				break;
			escaped = false;
			++end;
		}
	}
	next_start = end;
	if (q)
		++next_start;
	std::string ret("");
	ret.reserve(end - start);
	for (size_t i = start; i < end; i++)
	{
		if (std::find(escaped_indexes.begin(), escaped_indexes.end(), i) != escaped_indexes.end())
			continue;
		ret += str[i];
	}
	return ret;
}

char	**map_to_envp(const std::map<std::string, std::string> &map)
{
	char **envp = new char*[map.size() + 1];
	size_t i = 0;
	for (std::map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); ++it)
	{
		std::string tmp = it->first + "=" + it->second;
		envp[i] = new char[tmp.size() + 1];
		std::copy(tmp.begin(), tmp.end(), envp[i]);
		envp[i][tmp.size()] = 0;
		++i;
	}
	envp[i] = 0;
	return envp;
}

std::string utils::as_lower(const std::string &str)
{
	std::string ret(str);
	for (size_t i = 0; i < ret.size(); i++)
		ret[i] = std::tolower(ret[i]);
	return ret;
}