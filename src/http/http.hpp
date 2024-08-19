#pragma once
#ifndef HTTP_HPP
#define HTTP_HPP

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include <map>
#include <list>
#include <stdexcept>

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

	enum client_status {
    	GETTING_HEADER,
    	HEADER_ALL_RECEIVED,
    	GETTING_BODY,       // X This can all be done in parallel
    	BODY_ALL_RECEIVED,
    	TREATING_REQUEST, // X This can all be done in parallel
    	SENDING_RESPONSE, // X This can all be done in parallel
	};

	struct sub_request {
		std::map<std::string, std::string>	headers;
		std::string							body;
		size_t								body_size;
	};

	struct Request
	{
		enum Http::e_method					method;
		enum Http::e_protocol				protocol;
		std::string							host;
		std::string							uri;

		std::list<struct sub_request>		sub_requests;
		size_t								total_body_size;
		enum client_status					status;
	};

	struct Response
	{
		enum Http::e_method					method;
		enum Http::e_protocol				protocol;

		int									status_code; // 200, 404, 500, etc
		std::string							headers; // Raw string, just send it

		// IF file_path_to_send is not empty, send the file
		// (read path into buffer and write to socket directly)
		// Othwerwise, send body
		std::string							file_path_to_send;
		std::string							body; // Might be a char* instead, if using cgi(reduces overhead)
		char*								c_body;
		size_t								body_size;
	};

	class HttpException: public std::exception
	{
		private:
			int _status_code;
		public:
			HttpException(int status_code) : _status_code(status_code) {}
			int get_status_code() { return _status_code; }
	};

}

#endif // HTTP_HPP
