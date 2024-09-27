#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

#define BODY "\
import os\n\
import http.cookies\n\ndef print_html(cookie_value=None):\n    # Print content type header and set cookie if needed\n    print(\"Content-Type: text/html\", end=\"\r\n\", flush=True)\n    if cookie_value:\n        print(f\"Set-Cookie: user_id={cookie_value}; Path=/; HttpOnly\", end=\"\r\n\", flush=True)\n\n    print(\"\",end=\"\r\n\")  # End of headers (must have a blank line here to separate headers from body, end=\"\r\n\", flush=True)\n\n    # HTML content begins\n    print(\"<html><head><title>CGI Cookie Example</title></head>\", flush=True)\n    print(\"<body>\", flush=True)\n    if cookie_value:\n        print(f\"<p>Cookie 'user_id' is set to: {cookie_value}</p>\", flush=True)\n    else:\n        print(\"<p>No cookie found. Setting a new cookie!</p>\", flush=True)\n    print(\"</body></html>\", end=\"\r\n\", flush=True)\n\n# Read existing cookies\ncookie = http.cookies.SimpleCookie(os.environ.get(\"HTTP_COOKIE\", \"\"))\nuser_id = cookie.get(\"user_id\")\n\n# If there's no cookie, set a new one\nif user_id is None:\n    user_id = \"12345\"  # Example of a new user ID\n    print_html(cookie_value=user_id)\nelse:\n    # If cookie exists, display its value\n    print_html(cookie_value=user_id.value)\nexit(0)\n"

int main() {
    int pipe_in[2];
    int pipe_out[2];
    pid_t pid;

    // Create pipes
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        perror("pipe");
        return 1;
    }

    // Fork process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) { // Child process
        // Redirect stdin and stdout
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);

        // Close unused pipe ends
        close(pipe_in[1]);
        close(pipe_out[0]);

        // Execute python3
        char *tab[] = {strdup("/usr/bin/python3"), NULL};
        execve("/usr/bin/python3",  tab, 0);
        perror("execlp");
        return 1;
    } else { // Parent process
        // Close unused pipe ends
        close(pipe_in[0]);
        close(pipe_out[1]);

        // Send body to python3
        const char *body = BODY;
        write(pipe_in[1], body, strlen(body));
        close(pipe_in[1]);

        // Read response from python3
        char buffer[128];
        ssize_t count;
        while ((count = read(pipe_out[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[count] = '\0';
            std::cout << buffer;
        }
        close(pipe_out[0]);

        // Wait for child process to finish
        // waitpid(pid, nullptr, 0);
    }

    return 0;
}