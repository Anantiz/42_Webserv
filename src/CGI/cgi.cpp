#include "cgi.hpp"

CGI_bridge::CGI_bridge(Client &client, const std::string &cgi_path, const std::string &local_file_path)
{
    _pipe_output[0] = -1;
    _pipe_output[1] = -1;
    _pipe_input[0] = -1;
    _pipe_input[1] = -1;

    _args.push_back(ft_strjoin("", cgi_path.data())); // To malloc it
    _args.push_back(0);

    logs::SdevLog("Executing CGI: " + cgi_path + " with uri: " + client.request.uri);

    create_env(client, local_file_path);
    fork_exec(cgi_path);
    send_body(client);
    read_response(client);
}

static int ft_close(int *pfd)
{
    if (*pfd != -1) {
        int ret = ::close(*pfd);
        *pfd = -1;
        return ret;
    }
    return 0;
}

CGI_bridge::~CGI_bridge()
{
    for (size_t i = 0; _env[i]; i++)
        free((char *)_env[i]);

    for (size_t i = 0; _args[i]; i++)
        free((char *)_args[i]);

    ft_close(&_pipe_output[0]);
    ft_close(&_pipe_output[1]);
    ft_close(&_pipe_input[0]);
    ft_close(&_pipe_input[1]);
}

/*
Create thes Environment for the CGI:
*/
void CGI_bridge::create_env(Client &client, const std::string &local_file_path)
{
    _env.push_back(ft_strjoin("", "GATEWAY_INTERFACE=CGI/1.1"));
    _env.push_back(ft_strjoin("", "REDIRECT_STATUS=CGI"));
    _env.push_back(ft_strjoin("", "SERVER_PROTOCOL=HTTP/1.1"));
    _env.push_back(ft_strjoin("SCRIPT_FILENAME=", local_file_path.data()));
    _env.push_back(ft_strjoin("SERVER_NAME=", client.request.host.data()));

    switch (client.request.method)
    {
        case(Http::GET):
            _env.push_back(ft_strjoin("", "REQUEST_METHOD=GET"));
            break;
        case(Http::POST):
            break;
        default:
            logs::SdevLog("Unknown method in CGI");
            throw Http::HttpException(501);
    }
    _env.push_back(0);
}

void CGI_bridge::fork_exec(const std::string &cgi_path)
{
    if (pipe(_pipe_input) == -1)
        throw Http::HttpException(500);
    if (pipe(_pipe_output) == -1) {
        ft_close(&_pipe_input[1]); ft_close(&_pipe_input[0]);

        throw Http::HttpException(500);
    }

    int pid = fork();
    if (pid == -1) {
        ft_close(&_pipe_input[1]);ft_close(&_pipe_input[0]);
        ft_close(&_pipe_output[1]);ft_close(&_pipe_output[0]);
        throw Http::HttpException(500);
    }
    if (pid == 0)
    {
        dup2(_pipe_input[0], 0); // CGI input
        ft_close(&_pipe_input[0]);
        ft_close(&_pipe_input[1]);

        dup2(_pipe_output[1], 1); // CGI output
        ft_close(&_pipe_output[0]);
        ft_close(&_pipe_output[1]);

        execve(cgi_path.c_str(), _args.data(), _env.data());
        logs::SdevLog("\033[91mError\033[0m executing CGI: " + cgi_path);
        exit(1);
    }
    ft_close(&_pipe_input[0]);
    ft_close(&_pipe_output[1]);
}

void CGI_bridge::send_body(Client &client)
{
    size_t total_written = 0;

    while (total_written < client.request.body.size())
    {
        ssize_t ret = write(_pipe_input[1], &client.request.body.data()[total_written], CGI_READ_CHUNK);
        if (ret < 0) {
            logs::SdevLog("\033[91mError\033 writing to cgi[0m");
            ft_close(&_pipe_input[1]);ft_close(&_pipe_output[0]);
            throw Http::HttpException(500);
        }
        total_written += ret;
    }
    ft_close(&_pipe_input[1]);
}

void CGI_bridge::read_response(Client &client)
{
    size_t total_read = 0;
    char buff[CGI_READ_CHUNK] = {0};

    char *ret_buffer = ft_strjoin("", ""); // cuz join don't work with nullptr
    if (!ret_buffer)
        throw Http::HttpException(500);


    while (true)
    {
        ssize_t ret = read(_pipe_output[0], buff, CGI_READ_CHUNK);
        if (ret < 0) {
            logs::SdevLog("\033[91mError\033[0m reading from cgi");
            ft_close(&_pipe_output[0]);
            _pipe_output[0] = -1;
            throw Http::HttpException(500);
        }
        if (ret == 0)  // No more data to read
            break;

        total_read += (size_t)ret;
        if (total_read > 10000000) { // 10MB
            free(ret_buffer);
            ft_close(&_pipe_output[0]);
            throw Http::HttpException(500);
        }

        // Very inneficient, and will crash the server if the response is too big
        char *old = ret_buffer;
        ret_buffer = ft_strjoin(ret_buffer, buff);
        if (!ret_buffer) {
            free(old);
            throw Http::HttpException(500);
        }
        free(old);


        // There solved !
    }
    client.response.body = ret_buffer;
    free(ret_buffer);
    ft_close(&_pipe_output[0]);
    client.response.body_size = total_read;
    client.response.file_path_to_send.clear(); // We don't want to send a file

    client.response.status_code = 200;
    client.response.headers = Http::get_status_string(200);
    client.response.headers += "Content-Type: text/html\r\n";
    client.response.headers += "Content-Length: " + utils::anything_to_str(total_read) + "\r\n";
}
