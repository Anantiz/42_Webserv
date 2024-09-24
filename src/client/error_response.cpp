#include "client.hpp"
#include "utils/utils.hpp"
#include "utils/logs.hpp"

/**
 * If an error occured and the client could was not matched with a server
 * we build a basic error response and send it to the client.
 * It is otherwise the server's responsibility to build the response.
 */
void Client::error_response( const std::string& custom_page )
{
    if (response.status_code < 400) {
            logs::SdevLog("\033[91mWe are trying to build an error response with a status code that is not an error, how did we get here?\033[0m");
        return ;
    }

    if (custom_page.empty() or access(custom_page.c_str(), R_OK)) {
        if (!response.body.empty())
            logs::SdevLog("\033[91mWe are trying to build an error response with a body that is not empty, how did we get here? body: \033[0m" + response.body);
        response.file_path_to_send.clear();
        try {
            response.body = utils::build_error_page(response.status_code);
            response.headers = Http::get_status_string(response.status_code) + "Content-Type: text/html\r\n";
            response.headers += "Content-Length: " + utils::anything_to_str(response.body.size()) + "\r\n";
        }
        catch (std::exception &e) {
            // If the memory is so dead that the string concatenation fails just send 500 cuz the server is kinda dead at this point
            // String literals are always available even if the memory is dead
            response.status_code = 500;
            response.headers = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: 93\r\n";
            response.body = "<html><body><h1>500 Error: Our servers are going through a tough time !</h1></body></html>\r\n";
        }
    }
    else {
        response.body.clear();
        response.file_path_to_send = custom_page;
        response.headers = Http::get_status_string(response.status_code);
        response.headers += utils::get_content_type(custom_page);
        response.headers += utils::get_file_length_header(custom_page);
    }
    connection_status = RESPONSE_READY;
}