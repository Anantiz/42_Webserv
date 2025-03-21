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
#include <vector>

namespace Http {

	const std::string   &get_status_string(int status);


	enum e_method {
		UNKNOWN_METHOD = 0b0,
		GET = 0b1,
		POST = 0b10,
		DELETE = 0b100,
	};

	enum e_protocol {
		Others = 5,
		FALSE_PROTOCOL = 0b0,
		HTTP_1_0 = 0b1,
		HTTP_1_1 = 0b10,
		HTTP_2_0 = 0b100,
	};

	struct Boundary
	{
		std::string							startDelimiter;
		std::string							endDelimiter;

		// A vector of each part (header + body) of the multipart request
		// It's a pair with
		//     first: A map of the headers Header-Name -> Header-Value
		//     second: the body of the 
		std::vector<std::pair<std::map<std::string, std::string>, std::string> >	headBody;
	};

	struct Request
	{
		enum Http::e_method					method;
		enum Http::e_protocol				protocol;
		std::string							host;
		std::string							uri;
		std::map<std::string, std::string>	headers;
		size_t								body_size;
		size_t								received_size;
		std::string							body;

		std::map<std::string, std::string>	mainHeader;
		Http::Boundary						boundary;
		std::string							buffer;
		bool								multipart;

		// Parsed Headers
		bool								keep_alive;
		size_t								content_length;
		std::string							content_type;

	};

	struct Response
	{
		enum Http::e_method					method;
		int									status_code; // 200, 404, 500, etc
		std::string							headers; // Raw string, just send it

		// IF file_path_to_send is not empty, send the file
		// (read path into buffer and write to socket directly)
		// Othwerwise, send body
		std::string							file_path_to_send;
		int									file_fd;
		std::string							body; // Might be a char* instead, if using cgi(reduces overhead)
		size_t								body_size;

		// File buffer
		std::string							buffer;
		ssize_t								last_read;
		size_t								offset;
	};

  class HttpException: public std::exception
	{
		private:
			int _status_code;
		public:
			HttpException(int status_code) : _status_code(status_code) {}
			int get_status_code() const { return _status_code; }
  };
}

#endif // HTTP_HPP