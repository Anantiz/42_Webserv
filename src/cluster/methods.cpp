#include "cluster.hpp"

/**
 *  MESSAGE TO THE CORRECTOR:
 * 	    > I am fully aware that the subject says that two servers listening
 *        on the same port should be "forbidden", but since the subject also says
 *        to mimic Nginx behavior, I decided to allow it just like Nginx does.
 *      If you are not happy, go touch some grass. :)
 */
void	Cluster::match_request_serv(Client &client) const
{
	for (size_t p = 0; p < _servers_ports.size(); p++) {
		if (client.access_port == _servers_ports[p].first) {
			// If no host is provided, use the first server
			if (client.request.host.empty()) {
				client.server = _servers_ports[p].second[0].second;
				return;
			}
			// RFC 2616, section 3.2.3, states that the host is case-insensitive
			std::string request_host = utils::as_lower(client.request.host);
			// Find the server associated with the host, slow af striing comparison
			for (size_t i = 0; i < _servers_ports[p].second.size(); i++) {
				if (request_host == _servers_ports[p].second[i].first) {
					client.server = _servers_ports[p].second[i].second;
					return;
				}
			}
			// If host is not found, provide the first server
			client.server = _servers_ports[p].second[0].second;
		}
	}
}

void	Cluster::remove_closed_conections() {
	while (_to_remove.size()) {
		int i = _to_remove.back();
		_to_remove.pop_back();
		_client_pool.erase(_poll_fds[i].fd);
		_poll_fds.erase(_poll_fds.begin() + i);
		_client_count--;
	}
}

void	Cluster::build_error_response(Client *client)
{
	// Just to test
	const std::string const default_error_pages_path = "/home/aurban/Projects/webserv/src/http_error_pages/";
	Http::Response &res = client->response;

	if (res.status_code >= 400 && res.body.empty()) {
		res.file_path_to_send = "";
		try {
			res.body = utils::build_error_page(res.status_code);
		}
		catch (std::exception &e) { // If the memory is so dead that the string concatenation fails just send 500 cuz the server is kinda dead at this point
			res.status_code = 500;
			// String literals are always available even if the memory is dead
			res.headers = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: 93\r\n\r\n";
			res.body = "<html><body><h1>500 Error: Our servers are going through a rough time !</h1></body></html>\r\n";
		}
	}
	// We are only building an error reponse, wtf is this *brain-is-dead* ?

	//if (!res.file_path_to_send.empty()) { // Read the file yourself and send it as soon
	//	// [...]
	//}
	//else if (!res.body.empty()) { // Body has already been built, most likely CGI
	//	// [...]
	//}
	//else { // What the hell happened?
	//	_logger.devLog("Look at this, we're trying to send an error response without a body or a file path");
	//
	//}
}