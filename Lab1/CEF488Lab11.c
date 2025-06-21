#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
	int num_clients = 5;

	int client_number;
	pid_t child_pid;

	for (client_number =1; client_number <= num_clients; client_number ++) {
		child_pid = fork();

		if (child_pid < 0) {
			perror("fork failed");
			exit(1);
		}

		if (child_pid == 0) {
			printf("Child (Client %d): Handling the Request. My PID = %d, My parent's PID = %d.\n", client_number, getpid(), getppid());
			exit(0);
		}

		else {
			printf("Parent: Creating client: %d, Parent PID = %d Child PID = %d\n", client_number, getpid(), child_pid);
		}
	}

	for (int i = 1; i <= num_clients; ++i) {
		int status;
		pid_t wpid = wait(&status);

		printf("Parent: Child %d (PID %d) finished. Status = %d\n", i, wpid, status);
	}

	printf("Parent: All clients have been served. Exiting.\n");
	return 0;
}
