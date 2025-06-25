#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]){
	int sock = 0;
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE] = {0};
	const char *message = "Hello from Client!";

	if(argc < 2){
		fprintf(stderr, "Usage: %s 172.20.10.5\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	const char *server_ip = argv[1];

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Socket creation error");
		exit(EXIT_FAILURE);
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if(inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0){
		perror("Invalid address / Address not supported");
		exit(EXIT_FAILURE);
	}

	printf("Client connecting to %s:%d...\n", server_ip, PORT);
	if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("Connection Failed");
		exit(EXIT_FAILURE);
	}
	printf("Connected to server.\n");

	if(send(sock, message, strlen(message), 0) == -1){
		perror("send");
		close(sock);
		exit(EXIT_FAILURE);
	}
	printf("Client sent: \"%s\"\n", message);

	ssize_t bytes_read = read(sock, buffer, BUFFER_SIZE);
	if(bytes_read == -1){
		perror("read");
		close(sock);
		exit(EXIT_FAILURE);
	}
	else if(bytes_read == 0){
		printf("Server closed connection or sent no data.\n");
	}
	else {
		buffer[bytes_read] = '\0';
		printf("Server response: %s\n", buffer);
	}

	close(sock);

	printf("Client closed.\n");

	return 0;
}
