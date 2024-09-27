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
    send_body(client, local_file_path);
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
    client.request.headers = client.request.mainHeader; // Fuck you loris
    try {
        _env.push_back("GATEWAY_INTERFACE=CGI/1.1");
        _env.push_back("REDIRECT_STATUS=CGI");
        _env.push_back("SERVER_PROTOCOL=HTTP/1.1");
        _env.push_back("SCRIPT_FILENAME=" + local_file_path); // Not sure if it is right, this sends the path of the file being feed into the stdin, not the path of the executable
        _env.push_back("SERVER_NAME=" + client.request.host);
        _env.push_back("SERVER_SOFTWARE=Webserv");
        _env.push_back("SERVER_PORT=" + utils::anything_to_str(client.access_port));
        _env.push_back("REMOTE_ADDR=" + utils::anything_to_str(client.client_addr.sin_addr.s_addr));
        _env.push_back("REMOTE_PORT=" + utils::anything_to_str(client.client_addr.sin_port));
        _env.push_back("REQUEST_URI=" + client.request.uri);

        //Set cookies (from client.request.headers, it is a map)
        if (client.request.headers.find("Cookie") != client.request.headers.end()) {
            _env.push_back("HTTP_COOKIE=" + client.request.headers["Cookie"]);
            logs::SdevLog("Cookie: " + client.request.headers["Cookie"]);
        } else {
            logs::SdevLog("No cookie");
        }
        // Handle User-Agent
        if (client.request.headers.find("User-Agent") != client.request.headers.end()) {
            _env.push_back("HTTP_USER_AGENT=" + client.request.headers["User-Agent"]);
        }
        // Handle Referer
        if (client.request.headers.find("Referer") != client.request.headers.end()) {
            _env.push_back("HTTP_REFERER=" + client.request.headers["Referer"]);
        }
        // What is missing ? (We only support GET and POST)

        switch (client.request.method)
        {
            case(Http::GET): {
                _env.push_back("REQUEST_METHOD=GET");

                std::string::size_type pos = client.request.uri.find("?");
                if (pos != std::string::npos) {
                    _env.push_back("QUERY_STRING=" + client.request.uri.substr(pos + 1));
                } else {
                    _env.push_back("QUERY_STRING=");
                }

                break;
            }
            case(Http::POST): {
                _env.push_back("REQUEST_METHOD=POST");

                if (client.request.headers.find("Content-Type") == client.request.headers.end())
                    throw Http::HttpException(400);
                else
                    _env.push_back("CONTENT_TYPE=" + client.request.headers["Content-Type"]);

                if (client.request.headers.find("Content-Length") == client.request.headers.end())
                    throw Http::HttpException(400);
                else
                    _env.push_back("CONTENT_LENGTH=" + client.request.headers["Content-Length"]);

                break;
            }
            default:
                logs::SdevLog("Unknown method in CGI");
                throw Http::HttpException(501);
        }
    } catch (std::exception &e) {
        logs::SdevLog("\033[91mError\033[0m creating CGI environment: " + std::string(e.what()));
        throw Http::HttpException(500);
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

    _pid = fork();
    if (_pid == -1) {
        ft_close(&_pipe_input[1]);ft_close(&_pipe_input[0]);
        ft_close(&_pipe_output[1]);ft_close(&_pipe_output[0]);
        throw Http::HttpException(500);
    }
    if (_pid == 0)
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
            exit(1);
        }
        delete[] c_args;
        delete[] c_env;
        exit(1);
    }
    ft_close(&_pipe_input[0]);
    ft_close(&_pipe_output[1]);
}

void CGI_bridge::send_body(Client &client, const std::string &local_file_path)
{
    size_t total_written = 0;
    char buff[CGI_READ_CHUNK] = {0};
    int fd = -1;

    if (client.request.method == Http::GET)
    {
        fd = open(local_file_path.data(), O_RDONLY);
        if (fd == -1) {
            kill(_pid, SIGKILL);
            ft_close(&_pipe_input[1]);ft_close(&_pipe_output[0]);
            logs::SdevLog("\033[91mError\033[0m opening file: " + local_file_path);
            throw Http::HttpException(500);
        }
        logs::SdevLog("Opened file: " + local_file_path);
    }

    bool run = true;
    while (run)
    {
        ssize_t bytes_written = 0;

        if (client.request.method == Http::GET) {
            ssize_t bytes_read = read(fd, buff, CGI_READ_CHUNK);
            logs::SdevLog("Read from file: " + utils::anything_to_str(bytes_read) + " bytes");
            if (bytes_read < 0) {
                logs::SdevLog("\033[91mError\033[0m reading from file");
                close(fd);
                ft_close(&_pipe_input[1]);ft_close(&_pipe_output[0]);
                throw Http::HttpException(500);
            } if (bytes_read == 0) {
                run = false;
            }
            bytes_written = write(_pipe_input[1], buff, (size_t)bytes_read);
        } else {
            bytes_written = write(_pipe_input[1], &client.request.body.data()[total_written], CGI_READ_CHUNK);
            total_written += bytes_written;
            if (total_written >= client.request.body.size())
                run = false;
        }

        if (bytes_written < 0) {
            logs::SdevLog("\033[91mError\033 writing to cgi[0m");
            ft_close(&_pipe_input[1]);ft_close(&_pipe_output[0]);
            close(fd);
            throw Http::HttpException(500);
        }
    }
    ft_close(&_pipe_input[1]);
    close(fd);
}

#define TIMEOUT 5
void CGI_bridge::read_response(Client &client)
{
    time_t start_time = time(0);
    while (waitpid(_pid, 0, WNOHANG) == 0) {
        if (time(0) - start_time >= TIMEOUT) {
            logs::SdevLog("\033[91mError\033[0mCGI timeout");
            ft_close(&_pipe_output[0]);
            _pipe_output[0] = -1;
            throw Http::HttpException(500);
        }
        usleep(50);
    }

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
            logs::SdevLog("\033[91mError\033[0m CGI response too big");
            ft_close(&_pipe_output[0]);
            throw Http::HttpException(413);
        }
    }
    ft_close(&_pipe_output[0]);
    logs::SdevLog("Read CGI response: " + utils::anything_to_str(total_read) + " bytes");
    CGI_bridge::parse_cgi_response(client);
}

void CGI_bridge::parse_cgi_response(Client &client)
{
    std::string::size_type  pos = client.response.body.find("\r\n\r\n");

    if (pos == std::string::npos) {
        logs::SdevLog("\033[91mCGI response is missing headers\033[0m: " + client.response.body + ".");
        throw Http::HttpException(500);
    }
    std::string cgi_headers = client.response.body.substr(0, pos + 2);
    std::string cgi_body;
    if (pos + 4 < client.response.body.size())
        cgi_body = client.response.body.substr(pos + 4);
    else
        cgi_body = "";

    client.response.body = cgi_body;
    client.response.body_size = client.response.body.size();

    client.response.status_code = find_status_code(cgi_headers);
    if (client.response.status_code == 0)
        client.response.status_code = 200;
    client.response.headers = Http::get_status_string(client.response.status_code);
    client.response.headers += cgi_headers;

    client.response.file_path_to_send.clear(); // We don't want to send a file
    logs::SdevLog("CGI response ready: header_size=" + utils::anything_to_str(client.response.headers.size()) + " body_size=" + utils::anything_to_str( client.response.body_size));

}

int CGI_bridge::find_status_code(std::string &cgi_headers)
{
    int e = 0;

    std::string::size_type pos = cgi_headers.find("Status: ");
    if (pos == std::string::npos)
        return 0;
    std::string status_code = cgi_headers.substr(pos + 8, 3);
    return utils::str_to_int(status_code, e);
}