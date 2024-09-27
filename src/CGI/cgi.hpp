#include "client/client.hpp"
#include "http/http.hpp"
#include "utils/utils.hpp"

#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>

#define CGI_READ_CHUNK 16192

/*
Purpose:

    This class Receives a Client object:
        > It reads its Client.request and create the CGI environment variables.
        > Forks + execs the CGI script.
        > It sends the body to the CGI script.
        > It reads the response and Build the Client.response struct.
    I guess we can call it SlowCGI ...
*/
class CGI_bridge
{
    CGI_bridge() {}
public:

    CGI_bridge(Client &client, const std::string &cgi_path, const std::string &local_file_path);
    ~CGI_bridge();

private:
    std::vector<std::string>            _env;
    std::vector<std::string>            _args;
    int                                 _pipe_input[2];
    int                                 _pipe_output[2];
    int                                _pid;

    void create_env(Client &client, const std::string &local_file_path);
    void fork_exec(const std::string &cgi_path);
    void send_body(Client &client);
    void read_response(Client &client);
    static void parse_cgi_response(Client &client);
    static int find_status_code(std::string &cgi_headers);

};
