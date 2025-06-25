#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(){
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	socklen_t addrlen = sizeof(address);
	char buffer[BUFFER_SIZE] = {0};
	const char *hello_message = "Hello from Server!";

	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		perror("setsockopt");
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
		perror("listen");
		exit(EXIT_FAILURE);
	}

	printf("Server listening on port %d\n", PORT);

	if((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0){
		perror("accept");
		exit(EXIT_FAILURE);
	}
	printf("Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

	ssize_t bytes_read = read(new_socket, buffer, BUFFER_SIZE);
	if(bytes_read == -1){
		perror("read");
		close(new_socket);
		close(server_fd);
		exit(EXIT_FAILURE);
	}
	printf("Client message: %s\n", buffer);

	if(send(new_socket, hello_message, strlen(hello_message), 0) == -1){
		perror("send");
		close(new_socket);
		close(server_fd);
		exit(EXIT_FAILURE);
	}
	printf("Server sent: \"%s\"\n", hello_message);

	close(new_socket);
	close(server_fd);

	printf("Server closed.\n");

	return 0;
}
