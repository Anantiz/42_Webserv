#include "http.hpp"

/// @brief Send the content of a file to a socket
/// @param socket_fd the socket file descriptor to write to
/// @param path the path of the file to send
/// @return -1 for error, 0 for success
int	send_file_content(int socket_fd, const char *path) {
	// Very bad code
	char buffer[4096];

	int file_fd = open(path, O_RDONLY);
	if (file_fd == -1)
		return -1;
	while (true)
	{
		ssize_t read_size = read(file_fd, buffer, 4096);
		if (read_size < 1) {
			close(file_fd);
			return read_size;
		}

		// Sockets might not write everything at once
		ssize_t total_written = 0;
		while (total_written != read_size)
		{
			ssize_t written = ::send(socket_fd, buffer + total_written, read_size - total_written, MSG_DONTWAIT);
			if (written == -1) {
				close(file_fd);
				return -1;
			}
			total_written += written;
		}
	}
	return 0;
}
