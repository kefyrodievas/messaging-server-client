#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "../msg.h"
#include "../Lib/str.h"

#define PORT 8080

int send_all(int fd, char *buf, size_t *len);
void *receive(void *socket_fd);

// struct Message
// {
//     uint16_t len;
//     char name[16];
//     char data[BUFF_SIZE];
// };

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

    // struct Message msg;

    char name[16];
    printf("Enter name: ");
    scanf("%s", name);

    // memset(msg.name, 0, sizeof(msg.name));
    // memcpy(msg.name, name, sizeof(name));

    // char *hello = "Helloo";

    // int ret = send(socket_fd, hello, strlen(hello), 0);
    // if (ret < 0) {
    //     perror("Failed to send");
    // }

    char buffer[BUFF_SIZE];

    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive, (void *)&socket_fd);

    // char buffer[BUFF_SIZE];

    while (1) {
        // memset(buffer, 0, 4096);

        scanf("%s", buffer);

        // memset(msg.data, 0, sizeof(msg.data));
        // memcpy(msg.name, buffer, sizeof(buffer));

        // msg.len = strlen(msg.name) + strlen(msg.data) +2;

        size_t l = strlen(buffer);
        char *buff = format_msg(buffer, &l, NAME);
        size_t msg_len = strlen(buff);

        // fgets(buffer, sizeof(buffer), STDIN_FILENO);
        // write(socket_fd, buffer, strlen(buffer));
        int ret = send_all(socket_fd, buff, &msg_len);
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

    // int bytes;
    // bytes = read(socket_fd, buffer, sizeof(buffer));
    // printf("%s\n", buffer);

    while (1) {
        memset(buffer, 0, BUFF_SIZE);
        int bytesRecv = recv(socket_fd, buffer, BUFF_SIZE, 0);

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