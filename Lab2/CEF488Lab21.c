#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
	int fd[2];
	pid_t child_pid;
	int number_to_send = 42;
	int received_number;

	if (pipe(fd) == -1) {
		perror("pipe failed");
		return  1;
	
	}
	child_pid = fork();

	if (child_pid == -1) {
		perror("fork failed");
		return 1;
	}

	if (child_pid == 0) {
		close(fd[1]);

		read(fd[0], &received_number, sizeof(received_number));

		printf("Child: Received number = %d\n", received_number);

		close(fd[0]);

		exit(0);

	}
	else {
		close(fd[0]);

		printf("Parent: Sending number = %d\n", number_to_send, sizeof(number_to_send));

		close(fd[1]);

		wait(NULL);

		return 0;
	}
}
