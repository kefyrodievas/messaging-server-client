#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "../msg.h"
#include "../Lib/str.h"

#define PORT 8080

void *receive(void *socket_fd);

int main(int argc, char *argv[ ]) {
    char *ip_address;
    if (argc > 1)ip_address = argv[1];
    else ip_address = "127.0.0.1";

    // Creating a socket
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        // printf("\n Socket creation error \n");
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ip_address, &address.sin_addr) <= 0) {
        // printf( "\nInvalid address/ Address not supported \n");
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Connecting to server
    int status;
    status = connect(socket_fd, (struct sockaddr *)&address, sizeof(address));
    if (status < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    char *name = malloc(16);
    printf("Enter name: ");
    readline(stdin, name, 16);

    int ret = sendf(socket_fd, name, strlen(name), NAME);
    if (ret < 0) {
        perror("Failed to send");
    }

    char buffer[BUFF_SIZE];

    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive, (void *)&socket_fd);

    while (1) {
        readline(stdin, buffer, BUFF_SIZE);

        if (strcmp(substr(buffer, 0, 4), "NAME") == 0) {
            name = substr(buffer, 5, strlen(buffer));

            while (findf(name, ' ') > 0) {
                name = erase(name, findf(name, ' '), 1);
            }
            if (strlen(name) < 1) {
                printf("Can not change the name\n");
                continue;
            }

            int ret = sendf(socket_fd, name, strlen(name), NAME);
            if (ret < 0) {
                perror("Failed to send");
            }
            continue;
        }

        if (strcmp(substr(buffer, 0, 4), "ROOM") == 0) {
            name = substr(buffer, 5, strlen(buffer));

            int ret = sendf(socket_fd, name, strlen(name), ROOM);
            if (ret < 0) {
                perror("Failed to send");
            }
            continue;
        }

        int ret = sendf(socket_fd, buffer, strlen(buffer), MESSAGE);
        if (ret < 0) {
            perror("Failed to send");
        }
    }

    pthread_join(receive_thread, NULL);

    // Close the connected socket
    close(socket_fd);
    return 0;
}

void *receive(void *socket) {
    int socket_fd = *(int *)socket;
    char buffer[BUFF_SIZE];

    while (1) {
        memset(buffer, 0, BUFF_SIZE);
        char type;
        int bytesRecv = recvf(socket_fd, buffer, &type);
        // int bytesRecv = recv(socket_fd, buffer, BUFF_SIZE, 0);

        if (bytesRecv < 0) {
            perror("Connection issue");
            break;
        }

        if (bytesRecv == 0) {
            perror("Server disconnected");
            break;
        }

        // Display message
        printf("%s\n", buffer);
    }
    return NULL;
}