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
    if (this->connection_status == RESPONSE_READY or this->connection_status == SENDING_RESPONSE)
    {
        if (this->response_status == NONE)
        {
            this->response.last_read = 0;
            this->response.offset = 0;
            this->response_status = SENDING_HEADER;
            this->connection_status = SENDING_RESPONSE;
            this->response.headers += "\r\n";
            logs::SdevLog("Headers: " + this->response.headers);
        }

        if (this->response_status == SENDING_HEADER)
        {
            if (this->response.offset == this->response.buffer.size()) {
                this->response_status = HEADER_SENT;
            }
            ssize_t tmp = send(\
                this->poll_fd.fd, \
                this->response.headers.c_str() + this->response.offset, \
                this->response.headers.size() - this->response.offset, 0
                );
            if (tmp <= 0) {
                logs::SdevLog("Error sending headers, client closed connection");
                this->to_close = true;
                cleanup_for_next_request();
            }
            this->response.offset += tmp;
        }
        else if (this->response_status == DONE_SENDING)
            finalize_response();

        if (this->response_status == HEADER_SENT)
        {
            this->response.last_read = 0;
            this->response.offset = 0;

            // Send by file or by buffer ?
            if (this->response.file_path_to_send.empty()) // Send body
                this->response_status = SENDING_FROM_BUFFER;
            else // Send file
            {
                this->response.buffer.clear();
                this->response.buffer.reserve(LOCAL_FILE_READ_CHUNK_SIZE);
                this->response_status = SENDING_FROM_FILE;
                this->response.file_fd = open(this->response.file_path_to_send.c_str(), O_RDONLY);
                if (this->response.file_fd == -1)
                { // Not sure if that will work
                    logs::SdevLog("Error opening file to send, local error");
                    this->response.status_code = 500;
                    this->response.body.clear();
                    this->response.file_path_to_send.clear();
                    this->error_response("");
                    this->response_status = SENDING_FROM_BUFFER;
                } else {
                    logs::SdevLog("Sending file: " + this->response.file_path_to_send);
                }

            }
        }

        if (this->response_status == SENDING_FROM_BUFFER)
        {
            if (this->response.offset == this->response.buffer.size()) {
                this->response.offset = 0;
                finalize_response();
            }
            ssize_t tmp = send(\
                this->poll_fd.fd, \
                this->response.buffer.c_str() + this->response.offset, \
                this->response.buffer.size() - this->response.offset, 0
                );
            if (tmp <= 0) {
                logs::SdevLog("Error sending body, client closed connection");
                this->to_close = true;
                cleanup_for_next_request();
            }
            this->response.offset += tmp;

        }
        else if (this->response_status == SENDING_FROM_FILE)
        {
            // Read
            if ((size_t)this->response.last_read == this->response.offset) {
                this->response.offset = 0;
                this->response.last_read = read(this->response.file_fd, (char *)this->response.buffer.c_str(), LOCAL_FILE_READ_CHUNK_SIZE);
                if (this->response.last_read < 0) {
                    logs::SdevLog("Error reading body file, local error");
                    this->response.status_code = 500;
                    this->response.body.clear();
                    this->response.file_path_to_send.clear();
                    this->error_response("");
                    this->response_status = SENDING_FROM_BUFFER;
                }
                else if (this->response.last_read == 0) {
                    logs::SdevLog("EOF on file, sending end of response");
                    this->response.offset = 0;
                    finalize_response();
                    return ;
                }
                logs::SdevLog("File Content: " + this->response.buffer);
            }
            // Write
            ssize_t tmp = send(\
                this->poll_fd.fd, \
                this->response.buffer.c_str() + this->response.offset, \
                this->response.last_read - this->response.offset,
                MSG_DONTWAIT
                );
            if (tmp <= 0) {
                logs::SdevLog("Error sending body file, client closed connection");
                this->to_close = true;
                cleanup_for_next_request();
            }
            this->response.offset += tmp;
        }
    }
}

void    Client::finalize_response( void )
{
    this->response_status = DONE_SENDING;
    if (this->response.file_fd != -1)
        close(this->response.file_fd);
    this->response.file_fd = -1;
    ssize_t sent = send(this->poll_fd.fd, "\0", 1, MSG_DONTWAIT);
    if (sent <= 0) {
        logs::SdevLog("Error sending end of response, client closed connection");
        this->to_close = true;
        cleanup_for_next_request();
    }
    this->response.offset += sent;
    if (this->response.offset == 1)
    {
        if (this->to_close)
            this->connection_status = TO_CLOSE;
        else
            cleanup_for_next_request();
    }
}

void    Client::cleanup_for_next_request( void )
{
    if (this->response.file_fd != -1)
        close(this->response.file_fd);
    if (this->to_close) {
        this->connection_status = Client::TO_CLOSE;
        return ;
    }

    this->response.file_fd = -1;
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

    this->response.method = Http::UNKNOWN_METHOD;
    this->response.status_code = 200;
    this->response.headers.clear();
    this->response.file_path_to_send.clear();
    this->response.body.clear();
    this->response.body_size = 0;
    this->response.buffer.clear();
    this->response.last_read = 0;
    this->response.offset = 0;

    this->eor = Client::DONT;
    this->treating_status = Client::NOTHING;
    this->response_status = Client::NONE;
    this->state = Client::PARSING_HEADERS;
    this->connection_status = Client::KEEP_ALIVE;
}
