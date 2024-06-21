#include "serv.hpp"

#include <iostream>
#include <string>

int main()
{
	try {
		serv s("test", 8080, "./");
		s.start();

	}catch (std::runtime_error &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return 0;
}