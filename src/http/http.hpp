#pragma once
#ifndef HTTP_HPP
#define HTTP_HPP

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

#include "client/client.hpp"

namespace Http {
	enum e_method {
		GET = 0b1,
		POST = 0b10,
		DELETE = 0b100,
	};

	enum e_protocol {
		Others,
		HTTP_1_0 = 0b1,
		HTTP_1_1 = 0b10,
		HTTP_2_0 = 0b100,
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
}

#endif // HTTP_HPP
