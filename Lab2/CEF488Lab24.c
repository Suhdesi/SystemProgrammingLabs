#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
	int fd1[2], fd2[2];
	pid_t child_pid;
	int numbers_to_send[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	int num_elements = sizeof(numbers_to_send) / sizeof(numbers_to_send[0]);
	int received_numbers[num_elements];
	int phonenumber = 650984696;
	float average = 0.0; // Use float for accurate average
	
	if(pipe(fd1) == -1 || pipe(fd2) == -1) {
		perror("pipe failed");
		return 1;
	}

	child_pid = fork();

	if(child_pid == 0){
		close(fd1[1]);
		close(fd2[0]);

		read(fd1[0], received_numbers, sizeof(received_numbers));
		printf("Child: Received numbers from parent.\n");

		int sum = 0;
		for(int i = 0; i < num_elements; i++) {
			sum += received_numbers[i];
		}

		average = (float)sum / num_elements; // Calculate the average
		printf("Child: Average = %f\n", average); // Print the average
		write(fd2[1], &average, sizeof(average)); // Corrected data type
		printf("Child: Sent average to parent.\n");

		close(fd1[0]);
		close(fd2[1]);
		exit(0);
	} else {
		close(fd1[0]);
		close(fd2[1]);

		printf("Parent: Sending numbers to child.\n");
		write(fd1[1], numbers_to_send, sizeof(numbers_to_send));


		wait(NULL);

		read(fd2[0], &average, sizeof(average)); // Corrected data type
		printf("Parent: Received average = %f from child.\n", average); // Print the average
		printf("Buffer Size is: %d\n", num_elements);
		printf("Phone number: %d\n", phonenumber);
		close(fd1[1]);
		close(fd2[2]);
	}

	return 0;
}
