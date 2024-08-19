#pragma once
#ifndef HTTP_HPP
#define HTTP_HPP

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include <map>

namespace Http {

	enum e_method {
		GET = 0b1,
		POST = 0b10,
		DELETE = 0b100,
		UNKNOWN_METHOD = 0b0,
	};

	enum e_protocol {
		Others,
		HTTP_1_0 = 0b1,
		HTTP_1_1 = 0b10,
		HTTP_2_0 = 0b100,
		FALSE_PROTOCOL = 0b0,
	};

	enum e_conection_status {
	/**
	 * I don't know at all what to do with this
	 * most probaly useless feature
	 * we'll see
	 */
		REQUEST,
		RESPONSE,
		CLOSED,
		KEEP_ALIVE
	};

	struct Request
	{
		enum Http::e_method					method;
		enum Http::e_protocol				protocol;
		std::string							host;
		std::string							uri;

		std::map<std::string, std::string>	headers;
		std::string							body;
		size_t								body_size;

	};

	struct Response
	{
		enum Http::e_method					method;
		enum Http::e_protocol				protocol;

		std::map<std::string, std::string>	headers;
		std::string							body;
		size_t								body_size;
	};
	// Only when boundary
	struct Boundary
	{
		std::string							delimiter;
		std::map<std::string, std::string>	headers;
		std::string							body;
	};
}

#endif // HTTP_HPP
