#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
	const char *fifo_path = "/tmp/myfifo";

	// Create FIFO with read-write permissions
	if(mkfifo(fifo_path, 0666) == -1) {
		perror("mkfifo");
		return 1;
	}

	printf("FIFO created at %s\n", fifo_path);
	return 0;
}	
