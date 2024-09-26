#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <map>

#define PIPE_READ 0
#define PIPE_WRITE 1

void ft_putstr_fd(int fd, char* str) {
    int i = 0;
    while (str[i])
        i++;
    write(fd, str, i);
}

char* ft_strcpy(char* dst, const char* src) {
    int i;

    i = 0;
    while (src[i]) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
    return (dst);
}

size_t ft_strlen(const char* s) {
    size_t i;

    i = 0;
    while (s[i])
        i++;
    return (i);
}

struct CGIData {
    std::string path;
    std::string file;
    std::string method;
    std::string query;
    std::vector<char*> args;
    std::map<std::string, std::string> env;
    int input_pipe[2];
    int output_pipe[2];
    std::string output;
};

char** env_map_to_array(std::map<std::string, std::string>& map) {
    int i = 0;

    char** env = new char*[map.size() + 1];
    for (std::map<std::string, std::string>::iterator itr = map.begin(); itr != map.end(); itr++) {
        size_t var_str_size = itr->first.size() + itr->second.size() + 2; // FOO=BAR\0
        env[i] = new char[var_str_size];

        ft_strcpy(env[i], itr->first.data());
        ft_strcpy(&env[i][itr->first.size()], "=");
        ft_strcpy(&env[i][itr->first.size() + 1], itr->second.data());

        i++;
    }

    env[i] = 0;

    return env;
}

void process_cgi(CGIData& cgi) {
    if (access(cgi.path.data(), F_OK | X_OK) != 0) {
        std::cerr << "failed to find cgi executable\n";
        return;
    }

    if (pipe(cgi.input_pipe) < 0) {
        return;
    }

    if (pipe(cgi.output_pipe) < 0) {
        return;
    }

    int pid = fork();
    if (pid < 0) {
        std::cerr << "forked failed\n";
        return;
    }

    if (pid == 0) {
        dup2(cgi.input_pipe[PIPE_READ], STDIN_FILENO);
        dup2(cgi.output_pipe[PIPE_WRITE], STDOUT_FILENO);

        close(cgi.input_pipe[PIPE_WRITE]);

        close(cgi.output_pipe[PIPE_READ]);

        char** env = env_map_to_array(cgi.env);

        if (execve(cgi.path.data(), cgi.args.data(), env) == -1) {
            perror("execve");
        }
    } else {
        close(cgi.input_pipe[PIPE_READ]);
        close(cgi.output_pipe[PIPE_WRITE]);

        ft_putstr_fd(cgi.input_pipe[PIPE_WRITE], cgi.query.data());
        close(cgi.input_pipe[PIPE_WRITE]);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            char buf;
            while (read(cgi.output_pipe[PIPE_READ], &buf, 1)) {
                cgi.output += buf;
            }

            close(cgi.output_pipe[PIPE_READ]);

            std::cout << "CGI OUTPUT: [\n" << cgi.output << "]\n";

            return;
        }
    }
}

int main(int argc, char** argv, char** envp) {
    CGIData cgi;
    cgi.path = "/usr/bin/php-cgi";
    cgi.file = "cgi.php";
    cgi.query = "t=1";
    cgi.method = "POST";

    cgi.args.push_back(cgi.path.data());
    // cgi.args.push_back(cgi.query.data());
    cgi.args.push_back(0);

    cgi.env["GATEWAY_INTERFACE"] = "CGI/1.1";
    cgi.env["REDIRECT_STATUS"] = "CGI";
    cgi.env["SCRIPT_FILENAME"] = cgi.file;
    cgi.env["SERVER_NAME"] = "bear.com";
    cgi.env["SERVER_PROTOCOL"] = "HTTP/1.1";
    // cgi.env["PATH_INFO"] = "./"; // TODO: needs to have current directory
    cgi.env["REQUEST_METHOD"] = cgi.method;
    // cgi.env["QUERY_STRING"] = cgi.query; // NOTE: dont set this if we're making a POST request
    cgi.env["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
    cgi.env["CONTENT_LENGTH"] = "3"; // WHY IS IT THE LENGTH OF THE QUERY STRING

    process_cgi(cgi);

    return 0;
}