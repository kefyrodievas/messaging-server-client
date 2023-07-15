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

typedef struct {
    int socket;
    char *name;
    char *address;
} user;


int main(int argc, char **argv) {
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    // Creating socket file descriptor
    if (master_socket < 0) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    user client[MAX_CLIENTS];
    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    // freeaddrinfo(address);

    if (listen(master_socket, MAX_CLIENTS) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client[i].socket = 0;
        client[i].name = "";
        client[i].address = "";
    }

    socklen_t addrlen = sizeof(address);

    // char *buffer = (char*)malloc(BUFF_SIZE * sizeof(char));
    // memset(buffer, 0, sizeof(buffer));

    char buffer[BUFF_SIZE];

    char *hello = "Hello from server";

    int fd_count = 0;
    int fd_size = 5;
    // struct pollfd *poll_list;
    struct pollfd *poll_list = malloc(sizeof(*poll_list) * fd_size);

    poll_list[0].fd = master_socket;
    poll_list[0].events = POLLIN;
    fd_count++;

    while (1) {
        int poll_count = poll(poll_list, 1, -1);

        if (poll_count < 0) {
            perror("Poll failed");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < fd_count; i++) {

            // Check if someone's ready to read
            if (poll_list[i].revents & POLLIN) { // We got one!!

                if (poll_list[i].fd == master_socket) {
                    // If listener is ready to read, handle new connection

                    int socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
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

                    poll_list[fd_count].fd = socket;
                    poll_list[fd_count].events = POLLIN; // Check ready-to-read

                    fd_count++;
                    // add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

                    // printf("pollserver: new connection from %s on " "socket %d\n",
                    //     inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN),
                    //     newfd);

                }
                else {
                    // If not the listener, we're just a regular client
                    int nbytes = recv(poll_list[i].fd, buffer, BUFF_SIZE, 0);

                    int sender_fd = poll_list[i].fd;

                    printf("%s\n", buffer);
                    if (nbytes < 0) {

                    }

                    if (nbytes <= 0) {
                        // When client disconnects
                        if (nbytes == 0) {
                            // buffer[nbytes] = '\0';
                            for (int j = 0; j < MAX_CLIENTS; j++) {
                                if (socket == client[j].socket) continue;
                                if (client[j].socket == 0) continue;
                                send(client[j].socket, "Disconnected", 13, 0);
                            }

                            client[i].socket = 0;
                            client[i].address = NULL;
                        }
                        else {
                            perror("Receive failed");
                        }

                        close(poll_list[i].fd);

                        poll_list[i] = poll_list[fd_count - 1];

                        fd_count--;
                    }
                    else {
                        // We got some good data from a client

                        for (int j = 0; j < fd_count; j++) {
                            // buffer[read_value] = '\0';
                            // if (socket == poll_list[j].fd) continue;
                            // if (poll_list[j].fd == 0) continue;
                            // // buf = join((client[i].name + ": ").c_str(), (const char*)buf);
                            // send(client[j].socket, buffer, strlen(buffer), 0);

                            // Send to everyone!
                            int dest_fd = poll_list[j].fd;

                            // Except the listener and ourselves
                            if (dest_fd != master_socket && dest_fd != sender_fd) {
                                if (send(dest_fd, buffer, nbytes, 0) == -1) {
                                    perror("Failed to send");
                                }
                            }
                        }
                    }
                }
            }
        }
    }






    // int max_socket;
    // fd_set readfds;


    // while (1) {
    //     memset(buffer, 0, BUFF_SIZE);
    //     // buffer[0] = '\0';
    //     // buffer = "";
    //     FD_ZERO(&readfds);
    //     FD_SET(master_socket, &readfds);
    //     max_socket = master_socket;
    //     for (int i = 0; i < MAX_CLIENTS; i++) {
    //         int socket = client[i].socket;
    //         if (socket > 0) { FD_SET(socket, &readfds); }
    //         if (socket > max_socket) { max_socket = socket; }
    //     }

    //     select(max_socket + 1, &readfds, NULL, NULL, NULL);

    //     if (FD_ISSET(master_socket, &readfds)) {
    //         int socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    //         if (socket < 0) {
    //             perror("Accept failed");
    //             continue;
    //         }
    //         printf("New connection, socket: %d IP: %s\n", socket, inet_ntoa(address.sin_addr));
    //         send(socket, hello, strlen(hello), 0);

    //         for (int i = 0; i < MAX_CLIENTS; i++) {
    //             if (client[i].socket == 0) {
    //                 client[i].socket = socket;
    //                 client[i].address = inet_ntoa(address.sin_addr);
    //                 break;
    //             }
    //         }
    //     }

    //     for (int i = 0; i < MAX_CLIENTS; i++) {
    //         int socket = client[i].socket;
    //         if (!FD_ISSET(socket, &readfds)) { continue; }
    //         int read_value = recv(socket, buffer, BUFF_SIZE, 0);
    //         printf("%s\n", buffer);
    //         if (read_value < 0) {
    //             perror("Receive failed");
    //             continue;
    //         }

    //         // When client disconnects
    //         else if (read_value == 0) {
    //             buffer[read_value] = '\0';
    //             close(socket);
    //             for (int j = 0; j < MAX_CLIENTS; j++) {
    //                 if (socket == client[j].socket) continue;
    //                 if (client[j].socket == 0) continue;
    //                 send(client[j].socket, "Disconnected", 13, 0);
    //             }

    //             client[i].socket = 0;
    //             client[i].address = NULL;
    //         }
    //         // else if (std::string(buf).substr(0, 6) == "<name=") { // reads the username that is sent upon starting the client
    //         //     client[i].name = std::string(buf).substr(6, std::string(buf).find(">"));
    //         //     client[i].name.erase(client[i].name.find(">"));

    //         //     buf[read_value] = '\0';
    //         //     for (int j = 0; j < MAX_CLIENTS; j++) {
    //         //         if (socket == client[j].socket) continue;
    //         //         if (client[j].socket == 0) continue;
    //         //         buf = join((client[i].name + " -> ").c_str(), "Connected");
    //         //         send(client[j].socket, buf, strlen(buf), 0);
    //         //     }
    //         // }

    //         else {
    //             buffer[read_value] = '\0';
    //             for (int j = 0; j < MAX_CLIENTS; j++) {
    //                 if (socket == client[j].socket) continue;
    //                 if (client[j].socket == 0) continue;
    //                 // buf = join((client[i].name + ": ").c_str(), (const char*)buf);
    //                 send(client[j].socket, buffer, strlen(buffer), 0);
    //             }
    //         }
    //     }
    // }


    // closing the listening socket
    shutdown(master_socket, SHUT_RDWR);
    return 0;
}