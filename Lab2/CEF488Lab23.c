#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
	int fd1[2], fd2[2]; // Two pipes: fd1 (parent->child), fd2 (child->parent)
	pid_t child_pid;
	int numbers_to_send[] = {1, 2, 3, 4, 5};
	int num_elements = sizeof(numbers_to_send) / sizeof(numbers_to_send[0]);
	int received_numbers[num_elements];
	int sum = 0;

	// 1. Create the pipes
	if (pipe(fd1) == -1 || pipe(fd2) == -1) {
		perror("pipe failed");
		return -1;
	}

	// 2. Create the child process
	child_pid = fork();

	if(child_pid == -1) {
		perror("fork failed");
		return 1;
	}

	if(child_pid == 0){
		// Child process
		close(fd1[1]); // Close unused write end of pipe 1
		close(fd2[0]); // Close unused read end of pipe 2

		// 4a. Read the array from pipe 1
		read(fd1[0], received_numbers, sizeof(received_numbers));
		printf("Child: Received numbers from parent.\n");

		// 4b. Calculate the sum
		for (int i = 0; i < num_elements; i++){
			sum += received_numbers[i];
		}

		printf("Child: Sum = %d\n", sum);

		// 4c. Write the sum to pipe 2
		write(fd2[1], &sum, sizeof(sum));
		printf("Child: Sent sum to parent.\n");

		close(fd1[0]);
	 	close(fd2[1]);
		exit(0);
	} else {
		// Parent process
		close(fd1[0]); // Close unused read end of pipe 1
		close(fd2[0]); // Close unused write end of pipe 2
		
		// 3. Write the array to pipe 1
		printf("Parent: Sending numbers to child.\n");
		write(fd1[1], numbers_to_send, sizeof(numbers_to_send));

		// 5a. Wait for the child to finish processing
		wait(NULL);

		// 5b. Read the sum from pipe 2
		read(fd2[0], &sum, sizeof(sum));
		printf("Parent: Received sum = %d from child.\n", sum);

		close(fd1[1]);
		close(fd2[0]);
	}

	return 0;
}
