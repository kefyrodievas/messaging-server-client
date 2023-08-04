#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <poll.h>

#include "../msg.h"
#include "../Lib/str.h"

// #define PORT 8080
#define MAX_CLIENTS 30

void poll_list_add(struct pollfd *poll_list[ ], int newfd, int *fd_count, int *fd_size);
void poll_list_del(struct pollfd poll_list[ ], int i, int *fd_count);
// int get_listener(void);

typedef struct {
    int socket;
    char *name;
    char *address;
} user;

typedef struct {
    char *name;
    user users[MAX_CLIENTS];
} room;

int main(int argc, char *argv[ ]) {
    uint16_t port;
    if (argc > 1) port = strtoul(argv[1], NULL, 0);
    else port = 8080;

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
    address.sin_port = htons(port);

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

    user null_usr;
    null_usr.socket = 0;
    null_usr.name = NULL;
    null_usr.address = NULL;

    user client[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client[i] = null_usr;
    }


    // FILE *fptr;

    // // Create a file
    // fptr = fopen("../rooms1.txt", "w");

    // // Close the file
    // fclose(fptr);

    FILE *rooms_file;
    rooms_file = fopen("rooms.txt", "r");

    if (rooms_file == NULL) {
        printf("Failed to open rooms file\n");
        exit(0);
    }

    room rooms[20];
    int room_count = 0;

    char *room_name = malloc(16);
    for (room_count = 0; readline(rooms_file, room_name, 16) != EOF; room_count++) {
        printf("%s\n", room_name);
        rooms[room_count].name = strdup(room_name);
    }
    // for (room_count = 0; fgets(room_name, 16, rooms_file); room_count++) {
    //     printf("%s\n", room_name);
    //     rooms[room_count].name = strdup(room_name);
    // }

    // int listener = get_listener( );

    socklen_t addrlen = sizeof(address);

    // char *buffer = (char*)malloc(BUFF_SIZE * sizeof(char));
    // memset(buffer, 0, sizeof(buffer));

    char buffer[BUFF_SIZE];

    // char *hello = "Hello from server";

    int fd_count = 0;
    int fd_size = 5;
    struct pollfd *poll_list = malloc(sizeof(*poll_list) * fd_size);

    poll_list_add(&poll_list, listener, &fd_count, &fd_size);

    while (1) {
        int events = poll(poll_list, fd_count, -1);

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

                    client[socket].socket = socket;
                    client[socket].address = inet_ntoa(address.sin_addr);
                    client[socket].name = NULL;

                    rooms[0].users[socket] = client[socket];

                    // char *hello = "Hello. You are connected to room: ";
                    // size_t msg_len = strlen(rooms[0].name) + strlen(hello);
                    // char *tmp_buff = malloc(msg_len);
                    // memset(tmp_buff, 0, msg_len);
                    // strcpy(tmp_buff, hello);
                    // strcat(tmp_buff, rooms[0].name);
                    // sendf(socket, tmp_buff, strlen(tmp_buff), MESSAGE);

                    char *tmp_buff = malloc(BUFF_SIZE);
                    sprintf(tmp_buff, "Hello. You are connected to room: %s\n", rooms[0].name);

                    sendf(socket, tmp_buff, strlen(tmp_buff), MESSAGE);

                    poll_list_add(&poll_list, socket, &fd_count, &fd_size);

                    // printf("pollserver: new connection from %s on " "socket %d\n",
                    //     inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN),
                    //     newfd);

                }
                else {
                    int sender_fd = poll_list[i].fd;

                    // If not the listener, we're just a regular client
                    char type;
                    ssize_t nbytes = recvf(sender_fd, buffer, &type);

                    printf("%s\n", buffer);

                    if (nbytes <= 0) {
                        // When client disconnects
                        if (nbytes == 0) {
                            char *tmp_buff = malloc(BUFF_SIZE);
                            sprintf(tmp_buff, "%s disconnected", client[sender_fd].name);

                            for (int j = 0; j < fd_count; j++) {
                                int dest_fd = poll_list[j].fd;
                                if (dest_fd != listener && dest_fd != sender_fd) {
                                    if (sendf(dest_fd, tmp_buff, strlen(tmp_buff), MESSAGE) < 0) {
                                        perror("Failed to send");
                                    }
                                }
                            }
                        }
                        else {
                            perror("Receive failed");
                        }

                        client[sender_fd] = null_usr;

                        close(sender_fd);
                        poll_list_del(poll_list, i, &fd_count);
                    }
                    else {
                        char *tmp_buff = malloc(BUFF_SIZE + strlen(buffer));
                        room sender_room;

                        switch (type) {
                        case MESSAGE:
                            sprintf(tmp_buff, "%s: %s", client[sender_fd].name, buffer);

                            for (int j = 0; j < room_count; j++) {
                                if (rooms[j].users[sender_fd].socket == sender_fd) {
                                    sender_room = rooms[j];
                                    break;
                                }
                            }

                            for (int j = 0; j < MAX_CLIENTS; j++) {
                                if (sender_room.users[j].socket != 0) {
                                    int dest_fd = sender_room.users[j].socket;
                                    if (dest_fd != listener && dest_fd != sender_fd) {
                                        if (sendf(dest_fd, tmp_buff, strlen(tmp_buff), MESSAGE) < 0) {
                                            perror("Failed to send");
                                        }
                                    }
                                }
                            }

                            // for (int j = 0; j < fd_count; j++) {
                            //     // Send to everyone!
                            //     int dest_fd = poll_list[j].fd;

                            //     // Except the listener and ourselves
                            //     if (dest_fd != listener && dest_fd != sender_fd) {
                            //         if (sendf(dest_fd, tmp_buff, msg_len, MESSAGE) < 0) {
                            //             perror("Failed to send");
                            //         }
                            //     }
                            // }
                            break;

                        case NAME:
                            if (client[sender_fd].name == NULL) {
                                client[sender_fd].name = strdup(buffer);
                            }
                            else {
                                sprintf(tmp_buff, "%s is now known as %s\n", client[sender_fd].name, buffer);

                                for (int j = 0; j < room_count; j++) {
                                    if (rooms[j].users[sender_fd].socket == sender_fd) {
                                        sender_room = rooms[j];
                                        break;
                                    }
                                }

                                for (int j = 0; j < MAX_CLIENTS; j++) {
                                    if (sender_room.users[j].socket != 0) {
                                        int dest_fd = sender_room.users[j].socket;
                                        if (dest_fd != listener && dest_fd != sender_fd) {
                                            if (sendf(dest_fd, tmp_buff, strlen(tmp_buff), MESSAGE) < 0) {
                                                perror("Failed to send");
                                            }
                                        }
                                    }
                                }

                                // for (int j = 0; j < fd_count; j++) {
                                //     int dest_fd = poll_list[j].fd;
                                //     // Except the listener and ourselves
                                //     if (dest_fd != listener && dest_fd != sender_fd) {
                                //         if (sendf(dest_fd, tmp_buff, msg_len, MESSAGE) < 0) {
                                //             perror("Failed to send");
                                //         }
                                //     }
                                // }
                                client[sender_fd].name = strdup(buffer);
                            }
                            break;

                        case ROOM:

                            if (strcmp(buffer, "list") == 0) {
                                char *tmp_buff = malloc(20 * 16);
                                memset(tmp_buff, 0, 20 * 16);
                                for (int j = 0; j < room_count; j++) {
                                    strcat(tmp_buff, rooms[j].name);
                                    strcat(tmp_buff, "\n");
                                }
                                sendf(sender_fd, tmp_buff, strlen(tmp_buff), MESSAGE);
                                break;
                            }

                            int curr_room = -1;
                            int new_room = -1;

                            for (int j = 0; j < room_count; j++) {
                                if (rooms[j].users[sender_fd].socket == sender_fd) {
                                    curr_room = j;
                                }
                                if (strcmp(rooms[j].name, buffer) == 0) {
                                    new_room = j;
                                }
                            }
                            if (new_room == -1) {
                                char *err = "Room does not exist\n";
                                sendf(sender_fd, err, strlen(err), MESSAGE);
                                break;
                            }
                            if (curr_room == new_room) {
                                char *err = "You are already connected to this room\n";
                                sendf(sender_fd, err, strlen(err), MESSAGE);
                                break;
                            }

                            sprintf(tmp_buff, "You connected to room: %s\n", rooms[new_room].name);
                            sendf(sender_fd, tmp_buff, strlen(tmp_buff), MESSAGE);

                            sprintf(tmp_buff, "%s left", client[sender_fd].name);

                            for (int j = 0; j < room_count; j++) {
                                if (rooms[j].users[sender_fd].socket == sender_fd) {
                                    sender_room = rooms[j];
                                    break;
                                }
                            }

                            for (int j = 0; j < MAX_CLIENTS; j++) {
                                if (sender_room.users[j].socket != 0) {
                                    int dest_fd = sender_room.users[j].socket;
                                    if (dest_fd != listener && dest_fd != sender_fd) {
                                        if (sendf(dest_fd, tmp_buff, strlen(tmp_buff), MESSAGE) < 0) {
                                            perror("Failed to send");
                                        }
                                    }
                                }
                            }

                            rooms[new_room].users[sender_fd] = client[sender_fd];
                            rooms[curr_room].users[sender_fd] = null_usr;

                            sprintf(tmp_buff, "%s joined", client[sender_fd].name);

                            for (int j = 0; j < room_count; j++) {
                                if (rooms[j].users[sender_fd].socket == sender_fd) {
                                    sender_room = rooms[j];
                                    break;
                                }
                            }

                            for (int j = 0; j < MAX_CLIENTS; j++) {
                                if (sender_room.users[j].socket != 0) {
                                    int dest_fd = sender_room.users[j].socket;
                                    if (dest_fd != listener && dest_fd != sender_fd) {
                                        if (sendf(dest_fd, tmp_buff, strlen(tmp_buff), MESSAGE) < 0) {
                                            perror("Failed to send");
                                        }
                                    }
                                }
                            }
                            break;

                        default:
                            break;
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

// void echo(struct pollfd poll_list[ ], int fd_count, int listener) {
//     for (int j = 0; j < fd_count; j++) {
//         int dest_fd = poll_list[j].fd;

//         // Except the listener and ourselves
//         if (dest_fd != listener && dest_fd != sender_fd) {
//             if (sendf(dest_fd, buffer, nbytes, MESSAGE) < 0) {
//                 perror("Failed to send");
//             }
//         }
//     }
// }

void poll_list_add(struct pollfd *poll_list[ ], int newfd, int *fd_count, int *fd_size) {
    // If we don't have room, add more space in the poll list array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it

        *poll_list = realloc(*poll_list, sizeof(**poll_list) * (*fd_size));
    }

    (*poll_list)[*fd_count].fd = newfd;
    (*poll_list)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// Remove an index from the set
void poll_list_del(struct pollfd poll_list[ ], int i, int *fd_count) {
    // Copy the one from the end over this one
    poll_list[i] = poll_list[*fd_count - 1];

    (*fd_count)--;
}