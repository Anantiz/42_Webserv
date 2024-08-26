#include "cgi.hpp"

CGI_bridge::CGI_bridge(Client &client, const std::string &cgi_path)
{
    create_env(client);
    fork_exec(client, cgi_path);
    send_body(client);
    read_response(client);
}

CGI_bridge::~CGI_bridge()
{
    for (size_t i = 0; _env[i]; i++)
        free(_env[i]);
    free(_env);

    close(_pipe_read[0]);
}

/*
Create thes Environment for the CGI:

*/
void CGI_bridge::create_env(Client &client)
{

}

void CGI_bridge::fork_exec(Client &client, const std::string &cgi_path)
{
    if (pipe(_pipe_send) == -1) throw Http::HttpException(500);
    if (pipe(_pipe_read) == -1) {
        close(_pipe_send[1]); close(_pipe_send[0]);
        throw Http::HttpException(500);
    }
    int pid = fork();
    if (pid == -1) {
        close(_pipe_send[1]);close(_pipe_send[0]);
        close(_pipe_read[1]);close(_pipe_read[0]);
        throw Http::HttpException(500);
    }
    if (pid == 0)
    {
        dup2(_pipe_send[0], 0); // CGI input
        close(_pipe_send[0]);
        close(_pipe_send[1]);
        dup2(_pipe_read[1], 1); // CGI output
        close(_pipe_read[0]);
        close(_pipe_read[1]);
        exit(execve(cgi_path.c_str(), NULL, _env) == -1);
    }
    close(_pipe_send[0]);
    close(_pipe_read[1]);
}

void CGI_bridge::send_body(Client &client)
{
    size_t total_written = 0;
    while (total_written < client.request.body.size())
    {
        ssize_t ret = write(_pipe_send[1], &client.request.body.c_str()[total_written], 4096);
        if (ret == -2) { // Fd error
            close(_pipe_send[1]);close(_pipe_read[0]);
            throw Http::HttpException(500);
        }
        total_written += ret;
    }
    close(_pipe_send[1]);
}

void CGI_bridge::read_response(Client &client)
{
    size_t total_read = 0;
    while (true)
    {
        ssize_t ret = read(_pipe_read[0], &client.response.c_body[total_read], 4096);
        if (ret < 0) { // Fd error
            close(_pipe_read[0]);
            throw Http::HttpException(500);
        }
        if (ret == 0)  // No more data to read
            break;
        total_read += (size_t)ret;
    }
    close(_pipe_read[0]);
    client.response.body_size = total_read;
}
