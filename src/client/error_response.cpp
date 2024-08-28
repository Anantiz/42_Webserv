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
    if (custom_page.empty())
    {
        if (response.status_code >= 400 && response.body.empty()) {
            response.file_path_to_send = "";
            try {
                response.body = utils::build_error_page(response.status_code);
            }
            catch (std::exception &e) { // If the memory is so dead that the string concatenation fails just send 500 cuz the server is kinda dead at this point
                response.status_code = 500;
                // String literals are always available even if the memory is dead
                response.headers = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: 93\r\n\r\n";
                response.body = "<html><body><h1>500 Error: Our servers are going through a tough time !</h1></body></html>\r\n";
            }
        }
        else {
            // If the status code is not an error, we should not be here
            logs logger;
            logger.devLog("We are trying to build an error response with a status code that is not an error, how did we get here?");
        }
    }
    else
        response.body = custom_page;
    response.headers = Http::get_status_string(response.status_code) + "Content-Type: text/html\r\n";
    try {
        response.headers += "Content-Length: " + utils::ito_str(response.body.size()) + "\r\n\r\n";
    }
    catch (std::exception &e) {
        response.headers += "Content-Length: 0\r\n\r\n";
    }
    connection_status = RESPONSE_READY;
}