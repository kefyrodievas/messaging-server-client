#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <poll.h>

#include "str.h"

#define BUFF_SIZE 4096
#define PORT 8080
#define MAX_CLIENTS 30

void poll_add(struct pollfd *poll_list[ ], int newfd, int *fd_count, int *fd_size);
void poll_del(struct pollfd poll_list[ ], int i, int *fd_count);
// int get_listener(void);

typedef struct {
    int socket;
    char *name;
    char *address;
} user;

int main(int argc, char **argv) {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    // Creating socket file descriptor
    if (listener < 0) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(listener, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    // freeaddrinfo(address);

    if (listen(listener, MAX_CLIENTS) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    user client[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client[i].socket = 0;
        client[i].name = "";
        client[i].address = "";
    }

    // int listener = get_listener( );

    socklen_t addrlen = sizeof(address);

    // char *buffer = (char*)malloc(BUFF_SIZE * sizeof(char));
    // memset(buffer, 0, sizeof(buffer));

    char buffer[BUFF_SIZE];

    char *hello = "Hello from server";

    int fd_count = 0;
    int fd_size = 5;
    // struct pollfd *poll_list;
    struct pollfd *poll_list = malloc(sizeof(*poll_list) * fd_size);

    poll_list[0].fd = listener;
    poll_list[0].events = POLLIN;
    fd_count++;

    while (1) {
        int events = poll(poll_list, fd_count, 5000);

        if (events < 0) {
            perror("Poll failed");
            exit(EXIT_FAILURE);
        }

        if (events == 0) {
            printf("Poll timed out!\n");
        }

        for (int i = 0; i < fd_count; i++) {

            // Check if someone's ready to read
            if (poll_list[i].revents & POLLIN) { // We got one!!

                if (poll_list[i].fd == listener) {
                    // If listener is ready to read, handle new connection

                    int socket = accept(listener, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    if (socket < 0) {
                        perror("Accept failed");
                        continue;
                    }
                    printf("New connection, socket: %d IP: %s\n", socket, inet_ntoa(address.sin_addr));
                    send(socket, hello, strlen(hello), 0);

                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (client[i].socket == 0) {
                            client[i].socket = socket;
                            client[i].address = inet_ntoa(address.sin_addr);
                            break;
                        }
                    }

                    poll_add(&poll_list, socket, &fd_count, &fd_size);

                    // printf("pollserver: new connection from %s on " "socket %d\n",
                    //     inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN),
                    //     newfd);

                }
                else {
                    // If not the listener, we're just a regular client
                    int nbytes = recv(poll_list[i].fd, buffer, BUFF_SIZE, 0);

                    int sender_fd = poll_list[i].fd;

                    printf("%s\n", buffer);

                    if (nbytes <= 0) {
                        // When client disconnects
                        if (nbytes == 0) {
                            // buffer[nbytes] = '\0';

                            for (int j = 0; j < fd_count; j++) {
                                int dest_fd = poll_list[j].fd;
                                if (dest_fd != listener && dest_fd != sender_fd) {
                                    if (send(dest_fd, "Disconnected", 13, 0) < 0) {
                                        perror("Failed to send");
                                    }
                                }
                            }

                            client[i].socket = 0;
                            client[i].address = NULL;
                        }
                        else {
                            perror("Receive failed");
                        }

                        close(poll_list[i].fd);
                        poll_del(poll_list, i, &fd_count);
                    }
                    else {
                        for (int j = 0; j < fd_count; j++) {
                            // Send to everyone!
                            int dest_fd = poll_list[j].fd;

                            // Except the listener and ourselves
                            if (dest_fd != listener && dest_fd != sender_fd) {
                                if (send(dest_fd, buffer, nbytes, 0) < 0) {
                                    perror("Failed to send");
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // closing the listening socket
    shutdown(listener, SHUT_RDWR);
    return 0;
}

void poll_add(struct pollfd *poll_list[ ], int newfd, int *fd_count, int *fd_size) {
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it

        *poll_list = realloc(*poll_list, sizeof(**poll_list) * (*fd_size));
    }

    (*poll_list)[*fd_count].fd = newfd;
    (*poll_list)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// Remove an index from the set
void poll_del(struct pollfd poll_list[ ], int i, int *fd_count) {
    // Copy the one from the end over this one
    poll_list[i] = poll_list[*fd_count - 1];

    (*fd_count)--;
}