#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
	int fd1[2], fd2[2]; // Two pipes: fd1 (parent->child), fd2 (child->parent)
	pid_t child_pid;
	int number_to_send = 5;
	int received_number, processed_number;

	// 1. Create the pipes
	if (pipe(fd1) == -1 || pipe(fd2) == -1) {
		perror("pipe failed");
		return 1;
	}

	// 2. Create the child process
	child_pid = fork();

	if (child_pid == -1) {
		perror("fork failed");
		return 1;
	}

	if (child_pid == 0) {
		// Child process
		close(fd1[1]); // Close unused write end of pipe 1
		close(fd2[0]); // Close unused read end of pipe 2
		
		// 4a. Read the array from pipe 1
		read(fd1[0], &received_number, sizeof(received_number));
		printf("Child: Received %d from parent.\n", received_number);

		// 4b. Process the number
		processed_number = received_number * 2;
		printf("Child: Processed number = %d\n", processed_number);

		// 4c. Write to pipe 2
		write(fd2[1], &processed_number, sizeof(processed_number));
		printf("Child: Sent processed number to parent.\n");

			close(fd1[0]);
			close(fd2[1]);
			exit(0);
	} else {
		// Parent process
		close(fd1[0]); // Close unused read end of pipe 1
		close(fd2[1]); // Close unused write end of pipe 2
		
		// 3. Write to pipe 1
		printf("Parent: Sending %d to child.\n", number_to_send);
		write(fd1[1], &number_to_send, sizeof(number_to_send));

		// 5a. Wait for the child to finish writing
		wait(NULL);

		// 5b. Read from pipe 2
		read(fd2[0], &processed_number, sizeof(processed_number));
		printf("Parent: Received processed number = %d from child.\n", processed_number);

			close(fd1[1]);
			close(fd2[0]);
	}

	return 0;
}
