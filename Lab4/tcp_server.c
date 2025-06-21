#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For close()
#include <arpa/inet.h> // For sockaddr_in, inet_pton

#define PORT 65432
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char *message;
    ssize_t bytes_read;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <server_ip_address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *SERVER_IP = argv[1];

    // 1. Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure for the server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); // Convert port to network byte order

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address/Address not supported: %s\n", SERVER_IP);
        close(sock);
        exit(EXIT_FAILURE);
    }

    // 2. Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    while (1) {
        printf("Enter message to send (or 'exit' to quit): ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            // Error or EOF
            break;
        }

        // Remove trailing newline character if present
        buffer[strcspn(buffer, "\n")] = 0;

        message = buffer;

        if (strcmp(message, "exit") == 0) {
            break; // Exit loop if user types 'exit'
        }

        // 3. Send data to the server
        if (send(sock, message, strlen(message), 0) == -1) {
            perror("send failed");
            break;
        }

        // 4. Receive response from the server
        memset(buffer, 0, BUFFER_SIZE); // Clear the buffer
        bytes_read = recv(sock, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_read <= 0) {
            if (bytes_read == 0) {
                printf("Server disconnected.\n");
            } else {
                perror("recv failed");
            }
            break;
        }

        buffer[bytes_read] = '\0'; // Null-terminate the received data
        printf("Received from server: %s\n", buffer);
    }

    close(sock); // Close the client socket
    printf("Client disconnected.\n");
    return 0;
}
