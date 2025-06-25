#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"

int main(){
	int sock = 0;
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE] = {0};
	const char *message = "Hi from Client 2! Different data here.";

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Socket creation error");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if(inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0){
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("Connection Failed");
		return -1;
	}
	printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

	if(send(sock, message, strlen(message), 0) < 0){
		perror("Send failed");
		close(sock);
		return -1;
	}
	printf("Message sent: \"%s\"\n", message);

	int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
	if(bytes_received < 0){
		perror("Receive failed");
	}
	else if(bytes_received == 0){
		printf("Server closed connection.\n");
	}
	else{
		buffer[bytes_received] = '\0';
		printf("Response from server: \"%s\"\n", buffer);
	}

	close(sock);
	return 0;
}
