#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket, struct sockaddr_in client_addr){
	char buffer[BUFFER_SIZE];
	int bytes_received;

	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
	printf("New connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

	bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
	if(bytes_received < 0){
		perror("Error receiving data");
		close(client_socket);
		return;
	}
	buffer[bytes_received] = '\0';
	printf("Received from %s:%d: \"%s\"\n", client_ip, ntohs(client_addr.sin_port), buffer);

	const char *response = "Message received and acknowledged!";
	if(send(client_socket, response, strlen(response), 0) < 0){
		perror("Error sending response");
	}

	printf("Closed connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));
	close(client_socket);
}

int main(){
	int server_fd, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if(listen(server_fd, 3) < 0){
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	printf("Server listening on port %d...\n", PORT);

	while(1){
		if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0){
		perror("accept");
		continue;
		}

		pid_t pid = fork();
		if(pid < 0){
			perror("fork failed");
			close(new_socket);
			continue;
		}
		else if(pid == 0){
			close(server_fd);
			handle_client(new_socket, address);
			exit(0);
		}
		else{
			close(new_socket);
		}
	}

	close(server_fd);
	return 0;
}
