#include "utils.hpp"

const std::string	utils::ito_str(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}