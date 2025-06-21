#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	const char *fifo_path = "\tmp/myfifo";
	int fd;
	char buffer[1000];

	// Open FIFO for reading
	fd = open(fifo_path, O_RDONLY);
	if(fd == -1) {
		perror("open");
		return 1;
	}

	// Read data from FIFO
	read(fd, buffer, sizeof(buffer));
	printf("Client: Received message: %s", buffer);

	close(fd);
	return 0;
}
