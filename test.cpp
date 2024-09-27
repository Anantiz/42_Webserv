#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

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
        const char *body = "print('Hello from Python!')";
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