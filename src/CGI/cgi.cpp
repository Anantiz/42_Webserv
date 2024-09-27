#include "cgi.hpp"

CGI_bridge::CGI_bridge(Client &client, const std::string &cgi_path, const std::string &local_file_path)
{
    _pipe_output[0] = -1;
    _pipe_output[1] = -1;
    _pipe_input[0] = -1;
    _pipe_input[1] = -1;

    _args.push_back(cgi_path);

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
    _env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    _env.push_back("REDIRECT_STATUS=CGI");
    _env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    _env.push_back("SCRIPT_FILENAME=" + local_file_path);
    _env.push_back("SERVER_NAME=" + client.request.host);

    switch (client.request.method)
    {
        case(Http::GET):
            _env.push_back("REQUEST_METHOD=GET");
            break;
        case(Http::POST):
            break;
        default:
            logs::SdevLog("Unknown method in CGI");
            throw Http::HttpException(501);
    }
}

static char *cpp_strdup(std::string &s)
{
    char *ret = new char[s.size() + 1];
    std::copy(s.begin(), s.end(), ret);
    ret[s.size()] = 0;
    return ret;
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

        if (::access(cgi_path.data(), X_OK) == -1) {
            logs::SdevLog("\033[91mError\033[0m executing CGI: " + cgi_path);
            exit(1);
        }

        char **c_args = 0;
        char **c_env = 0;
        try {
            c_args = new char*[_args.size() + 1];
            for (size_t i = 0; i < _args.size(); i++)
                c_args[i] = cpp_strdup(_args[i]);
            c_args[_args.size()] = 0;

            c_env = new char*[_env.size() + 1];
            for (size_t i = 0; i < _env.size(); i++)
                c_env[i] = cpp_strdup(_env[i]);
            c_env[_env.size()] = 0;
            execve(cgi_path.data(), c_args, c_env);
        }
        catch (std::exception &e) {
            logs::SdevLog("\033[91mError\033[0m executing CGI: " + std::string(e.what()));
        }
        delete[] c_args;
        delete[] c_env;
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
    std::string tmp_buffer;
    char buff[CGI_READ_CHUNK] = {0};

    while (true)
    {
        ssize_t n_read = read(_pipe_output[0], buff, CGI_READ_CHUNK);
        if (n_read < 0) {
            logs::SdevLog("\033[91mError\033[0m reading from cgi");
            ft_close(&_pipe_output[0]);
            _pipe_output[0] = -1;
            throw Http::HttpException(500);
        }
        if (n_read == 0)  // No more data to read
            break;

        // Very inneficient, and will crash the server if the response is too big
        client.response.body += std::string(buff, n_read);

        // There solved !
        total_read += (size_t)n_read;
        if (total_read > 10000000) { // 10MB
            ft_close(&_pipe_output[0]);
            throw Http::HttpException(500);
        }
    }
    ft_close(&_pipe_output[0]);
    client.response.body_size = client.response.body.size();
    client.response.file_path_to_send.clear(); // We don't want to send a file

    client.response.status_code = 200;
    client.response.headers = Http::get_status_string(200);
    client.response.headers += "Content-Type: text/html\r\n";
    client.response.headers += "Content-Length: " + utils::anything_to_str(total_read) + "\r\n";
    client.response.headers += "Set-Cookie: " + utils::anything_to_str(total_read) + "\r\n";
}
