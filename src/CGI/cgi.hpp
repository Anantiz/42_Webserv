#include "client/client.hpp"
#include "http/http.hpp"

#include <string>
#include <map>

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
public:

    CGI_bridge(Client &client, const std::string &cgi_path);
    ~CGI_bridge();

private:
    std::map<std::string, std::string>  _env_map;
    char                                **_env;
    int                                 _pipe_send[2];
    int                                 _pipe_read[2];

    void create_env(Client &client);
    void fork_exec(Client &client, const std::string &cgi_path);
    void send_body(Client &client);
    void read_response(Client &client);

};
