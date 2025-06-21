#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(){
	int num_clients = 5;

	int client_number;
	pid_t child_pid;

	for (client_number = 1; client_number <= num_clients; client_number++) {
		child_pid = fork();

		if (child_pid < 0) {
			perror("fork failed");
			fprintf(stderr, "Parent: Failed to create client %d. Continuing... \n", client_number);
			continue;
		}

		if (child_pid == 0) {
			printf("Child (Client %d): Request Complete. Exiting. My PID = %d\n",	client_number, getpid());
			exit(0);
		}

		else {
			printf("Parent: Creating client: %d, Parent PID = %d Child PID = %d\n", client_number, getpid(), child_pid);
		}
	}

	for (int i = 1; i <= num_clients; ++i) {
		int status;
		pid_t wpid = wait(&status);
		if (wpid != 1) {
			printf("Parent: Child (PID %d) finished. Status = %d\n", wpid, status);
		}

		else {
			perror("wait failed");
		}

		printf("Parent: All clients have been served (or attempted). Exiting. \n");
		return 0;

	}
}

