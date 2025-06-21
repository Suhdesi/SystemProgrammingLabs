#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	const char *fifo_path = "/tmp/myfifo";
	int fd;
	char *message = "Hello from the server!\n";

	// Open FIFO for writing
	fd = open(fifo_path, O_WRONLY);
	if(fd == -1) {
		perror("open");
		return 1;
	}

	// Write to FIFO
	write(fd, message, sizeof(char) * 24); // including newline and null character
	printf("Server: Message written to FIFO.\n");

	close(fd);
	return 0;
}
