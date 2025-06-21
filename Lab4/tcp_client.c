#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For close()
#include <arpa/inet.h> // For sockaddr_in, inet_ntoa
#include <sys/socket.h> // For socket(), bind(), listen(), accept()
#include <pthread.h> // For threading (POSIX threads)

#define PORT 65432
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10 // Max concurrent clients

// Structure to pass client information to the thread
typedef struct {
    int client_sock;
    struct sockaddr_in client_addr;
} client_info_t;

// Function to handle each client connection in a separate thread
void *handle_client(void *arg) {
    client_info_t *client_info = (client_info_t *)arg;
    int client_sock = client_info->client_sock;
    struct sockaddr_in client_addr = client_info->client_addr;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Connected by client %s:%d\n", client_ip, ntohs(client_addr.sin_port));

    while (1) {
        memset(buffer, 0, BUFFER_SIZE); // Clear the buffer
        bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                printf("Client %s:%d disconnected.\n", client_ip, ntohs(client_addr.sin_port));
            } else {
                perror("recv failed");
            }
            break; // Exit loop on disconnect or error
        }

        buffer[bytes_received] = '\0'; // Null-terminate the received data
        printf("Received from %s:%d: %s\n", client_ip, ntohs(client_addr.sin_port), buffer);

        // Send an echo response back to the client
        char response[BUFFER_SIZE + 25]; // +25 for "Server received your message: "
        snprintf(response, sizeof(response), "Server received your message: %s", buffer);
        if (send(client_sock, response, strlen(response), 0) == -1) {
            perror("send failed");
            break;
        }
    }

    close(client_sock); // Close the client socket
    free(client_info); // Free the allocated memory for client info
    pthread_exit(NULL); // Terminate the thread
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    pthread_t tid;

    // 1. Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to allow immediate reuse of address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces
    server_addr.sin_port = htons(PORT); // Convert port to network byte order

    // 2. Bind the socket to the specified IP and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        client_addr_len = sizeof(client_addr);
        // 4. Accept a new connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
            perror("accept failed");
            continue; // Continue listening for other connections
        }

        // Allocate memory for client info and populate it
        client_info_t *client_info = (client_info_t *)malloc(sizeof(client_info_t));
        if (client_info == NULL) {
            perror("malloc failed for client_info");
            close(new_socket);
            continue;
        }
        client_info->client_sock = new_socket;
        client_info->client_addr = client_addr;

        // Create a new thread to handle the client
        if (pthread_create(&tid, NULL, handle_client, (void *)client_info) != 0) {
            perror("pthread_create failed");
            free(client_info); // Free memory if thread creation fails
            close(new_socket);
        }
        pthread_detach(tid); // Detach the thread so its resources are automatically reclaimed
    }

    // This part is typically not reached in an infinite loop server
    close(server_fd);
    return 0;
}
