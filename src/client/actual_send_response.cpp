#include "client.hpp"


#define LOCAL_FILE_READ_CHUNK_SIZE 16192

/**
 * Protocol:
 * `Headers` String should not contain the header-body separator, we add it here for simplicity
 *
 *
*/

void	Client::send_response( void )
{
    // ## 1: Validate the response
    if (this->connection_status == RESPONSE_READY or this->connection_status == SENDING_RESPONSE)
    {
        if (this->response_status == NONE) {
            this->response.last_read = 0;
            this->response.offset = 0;
            if (!this->response.file_path_to_send.empty() && access(this->response.file_path_to_send.c_str(), R_OK) == -1) {
                logs::SdevLog("\033[91mError\033[0m accessing file to send, local error file path : " + this->response.file_path_to_send);
                this->response.status_code = 404;
                this->response.body.clear();
                this->response.file_path_to_send.clear();
                this->generate_quick_response("");
            }
            if (this->response.headers.find("Content-Length:") == std::string::npos) {
                this->response.headers += "Content-Length: " + utils::anything_to_str(this->response.body.size()) + "\r\n";
                logs::SdevLog("Adding Content-Length to headers: ");
            }

            if (this->response.headers.find("\r\n\r\n") == std::string::npos) {
                this->response.headers += "\r\n"; // Add the separator between headers and body
                if (this->response.headers.find("\r\n\r\n") == std::string::npos) {
                    this->response.headers += "\r\n"; // Safety net if we forgot to put a\r\n after the last header
                }
            }

            this->response_status = SENDING_HEADER;
            this->connection_status = SENDING_RESPONSE;
            logs::SdevLog("Headers: \n" + this->response.headers);
        }

        // ## 2: Send headers of the response
        if (this->response_status == SENDING_HEADER) {
            if (this->response.offset == this->response.buffer.size()) {
                this->response_status = HEADER_SENT;
            }
            ssize_t tmp = send(\
                this->poll_fd.fd, \
                this->response.headers.c_str() + this->response.offset, \
                this->response.headers.size() - this->response.offset, 0
                );
            if (tmp <= 0) {
                logs::SdevLog("\033[91mError\033[0m sending headers, client closed connection");
                this->to_close = true;
                cleanup_for_next_request();
                return;
            }
            this->response.offset += tmp;
        }
        else if (this->response_status == DONE_SENDING) {
            finalize_response();
            return ;
        }

        // ## 3 Decide how to send the body
        if (this->response_status == HEADER_SENT)
        {
            this->response.last_read = 0;
            this->response.offset = 0;
            if (!this->response.body.empty()) // If body is present (CGI or errors)
                this->response_status = SENDING_PRE_FILLED_BODY;
            else if (!this->response.file_path_to_send.empty())// Otherwise Send file
            {
                this->response.buffer.clear();
                this->response.buffer.reserve(LOCAL_FILE_READ_CHUNK_SIZE);
                this->response_status = SENDING_FROM_FILE;
                this->response.file_fd = open(this->response.file_path_to_send.c_str(), O_RDONLY);
                if (this->response.file_fd == -1) {
                    logs::SdevLog("\033[91mError\033[0m opening file to send, local error file path : " + this->response.file_path_to_send);
                    this->connection_status = TO_CLOSE;
                    return ;
                } else
                    logs::SdevLog("Sending file: " + this->response.file_path_to_send);
            } else { // Nothing to send
                finalize_response();
                return ;
            }
        }

        // ## 3.1: Send the body (pre-filled)
        if (this->response_status == SENDING_PRE_FILLED_BODY)
        {
            if (this->response.offset == this->response.body.size()) {
                this->response.offset = 0;
                finalize_response();
                return ;
            }
            ssize_t tmp = send(\
                this->poll_fd.fd, \
                this->response.body.c_str() + this->response.offset, \
                this->response.body.size() - this->response.offset, 0
                );
            if (tmp <= 0) {
                logs::SdevLog("\033[91mError\033[0m sending body, client closed connection");
                this->to_close = true;
                cleanup_for_next_request();
                return ;
            }
            this->response.offset += tmp;
        }
        // ## 3.2: Send the body (from file)
        else if (this->response_status == SENDING_FROM_FILE)
        {
            // Read
            if ((size_t)this->response.last_read == this->response.offset) {
                this->response.offset = 0;
                this->response.last_read = read(this->response.file_fd, (char *)this->response.buffer.c_str(), LOCAL_FILE_READ_CHUNK_SIZE);
                if (this->response.last_read < 0) {
                    logs::SdevLog("\033[91mError\033[0m reading body file, local error");
                    this->response.status_code = 500;
                    this->response.body.clear();
                    this->response.file_path_to_send.clear();
                    this->generate_quick_response("");
                    this->response_status = SENDING_PRE_FILLED_BODY;
                }
                else if (this->response.last_read == 0) {
                    logs::SdevLog("EOF on file, sending end of response");
                    this->response.offset = 0;
                    finalize_response();
                    return ;
                }
            }
            // Write
            ssize_t tmp = send(\
                this->poll_fd.fd, \
                this->response.buffer.c_str() + this->response.offset, \
                this->response.last_read - this->response.offset,
                MSG_DONTWAIT
                );
            if (tmp <= 0) {
                logs::SdevLog("\033[91mError\033[0m sending body file, client closed connection");
                this->to_close = true;
                cleanup_for_next_request();
            }
            logs::SdevLog("Sent: " + utils::anything_to_str(tmp) + " bytes");
            this->response.offset += tmp;
        }
    }
}

void    Client::finalize_response( void )
{
    this->response_status = DONE_SENDING;
    if (this->response.file_fd != -1) {
        close(this->response.file_fd);
        this->response.file_fd = -1;
    }
    cleanup_for_next_request();
    logs::SdevLog("\033[96mResponse sent\033[0m");
}

void    Client::cleanup_for_next_request( void )
{
    if (this->response.file_fd != -1) {
        this->response.file_fd = -1;
        close(this->response.file_fd);
    }
    if ((this->to_close and this->request.keep_alive == false) or this->connection_status == TO_CLOSE) {
        this->connection_status = Client::TO_CLOSE;
        logs::SdevLog("\033[92mKilling\033[0m");
        return ;
    } else {
        this->connection_status = Client::KEEP_ALIVE;
        logs::SdevLog("\033[92mKeep-Alive\033[0m");
    }

    this->request.method = Http::UNKNOWN_METHOD;
    this->request.protocol = Http::FALSE_PROTOCOL;
    this->request.host.clear();
    this->request.uri.clear();
    this->request.headers.clear();
    this->request.body_size = 0;
    this->request.received_size = 0;
    this->request.body.clear();
    this->request.mainHeader.clear();
    this->request.boundary.startDelimiter.clear();
    this->request.boundary.endDelimiter.clear();
    this->request.boundary.headBody.clear();
    this->request.buffer.clear();
    this->request.multipart = false;

    this->request.keep_alive = false;
	this->request.content_length = 0;
	this->request.content_type.clear();

    this->response.method = Http::UNKNOWN_METHOD;
    this->response.status_code = 200;
    this->response.headers.clear();
    this->response.file_path_to_send.clear();
    this->response.body.clear();
    this->response.body_size = 0;
    this->response.buffer.clear();
    this->response.last_read = 0;
    this->response.offset = 0;

    this->to_close = false;
    this->eor = Client::DONT;
    this->treating_status = Client::NOTHING;
    this->response_status = Client::NONE;
    this->state = Client::PARSING_HEADERS;
}
