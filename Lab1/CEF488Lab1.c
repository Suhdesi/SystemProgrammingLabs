#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
	pid_t child_pid;

	printf("Server: Simulating a client connection... \n");

	child_pid = fork();

	if (child_pid < 0){
		perror("fork failed");
		return 1;
	}

	else if (child_pid == 0){
		printf("Child: This is the client. I am handling the request. My PID = %d, My parent's PID = %d.\n", getpid(), getppid());

	sleep(2);

	printf("Child: Request handled successfully. Exiting.\n");
	exit(0);
	}

	else {
		printf("Server: I have created a child process to handle a client. Parent PID = %d, Child PID = %d.\n", getpid(), child_pid);

	wait(NULL);
	}

	printf("Server: Child process with PID = %d has finished. Exiting.\n", child_pid);

	return 0;

}
