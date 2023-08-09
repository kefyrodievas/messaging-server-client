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
} client;

typedef struct {
    char *name;
    client clients[MAX_CLIENTS];
} room;

void echo_all(const char *buffer, struct pollfd poll_list[ ], int fd_count, int sender, int listener);
void echo_room(const char *buffer, room rooms[ ], int room_count, int sender, int listener);

int main(int argc, char *argv[ ]) {
    uint16_t port;
    if (argc > 1) port = (uint16_t)strtoul(argv[1], NULL, 0);
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

    client null_usr;
    null_usr.socket = 0;
    null_usr.name = NULL;
    null_usr.address = NULL;

    client clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i] = null_usr;
    }

    FILE *rooms_file;
    rooms_file = fopen("rooms.txt", "r");

    if (rooms_file == NULL) {
        printf("Failed to open rooms file\n");
        exit(EXIT_FAILURE);
    }

    room rooms[20];
    int room_count = 0;

    char room_name[16];
    for (room_count = 0; readline(rooms_file, room_name, 16) != EOF; room_count++) {
        printf("%s\n", room_name);
        rooms[room_count].name = strdup(room_name);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            rooms[room_count].clients[i] = null_usr;
        }
    }
    fclose(rooms_file);

    // int listener = get_listener( );

    socklen_t addrlen = sizeof(address);

    // char *buffer = (char*)malloc(BUFF_SIZE * sizeof(char));
    // memset(buffer, 0, sizeof(buffer));

    char buffer[BUFF_SIZE];

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

                    clients[socket].socket = socket;
                    clients[socket].address = inet_ntoa(address.sin_addr);
                    clients[socket].name = NULL;

                    rooms[0].clients[socket] = clients[socket];

                    char *tmp_buff = malloc(BUFF_SIZE);
                    sprintf(tmp_buff, "Hello. You are connected to room: %s\n", rooms[0].name);

                    sendf(socket, tmp_buff, strlen(tmp_buff), MESSAGE);

                    free(tmp_buff);

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
                            sprintf(tmp_buff, "%s disconnected", clients[sender_fd].name);

                            echo_all(tmp_buff, poll_list, fd_count, sender_fd, listener);

                            free(tmp_buff);
                        }
                        else {
                            perror("Receive failed");
                        }

                        clients[sender_fd] = null_usr;

                        close(sender_fd);
                        poll_list_del(poll_list, i, &fd_count);
                    }
                    else {
                        char *tmp_buff = malloc(BUFF_SIZE + strlen(buffer));

                        switch (type) {
                        case MESSAGE:
                            sprintf(tmp_buff, "%s: %s", clients[sender_fd].name, buffer);

                            echo_room(tmp_buff, rooms, room_count, sender_fd, listener);

                            // free(tmp_buff);
                            break;

                        case NAME:
                            if (clients[sender_fd].name == NULL) {
                                clients[sender_fd].name = strdup(buffer);

                                sprintf(tmp_buff, "%s connected", clients[sender_fd].name);
                                echo_all(tmp_buff, poll_list, fd_count, sender_fd, listener);
                            }
                            else {
                                sprintf(tmp_buff, "%s is now known as %s\n", clients[sender_fd].name, buffer);

                                echo_room(tmp_buff, rooms, room_count, sender_fd, listener);

                                clients[sender_fd].name = strdup(buffer);
                            }
                            // free(tmp_buff);
                            break;

                        case ROOM:

                            if (strcmp(buffer, "list") == 0) {
                                memset(tmp_buff, 0, 20 * 16);
                                for (int j = 0; j < room_count; j++) {
                                    strcat(tmp_buff, rooms[j].name);
                                    strcat(tmp_buff, "\n");
                                }
                                sendf(sender_fd, tmp_buff, strlen(tmp_buff), MESSAGE);
                                // free(tmp_buff);
                                break;
                            }

                            int curr_room = -1;
                            int new_room = -1;

                            for (int j = 0; j < room_count; j++) {
                                if (rooms[j].clients[sender_fd].socket == sender_fd) {
                                    curr_room = j;
                                }
                                if (strcmp(rooms[j].name, buffer) == 0) {
                                    new_room = j;
                                }
                            }
                            if (new_room == -1) {
                                char err[ ] = "Room does not exist\n";
                                sendf(sender_fd, err, strlen(err), MESSAGE);
                                break;
                            }
                            if (curr_room == new_room) {
                                char err[ ] = "You are already connected to this room\n";
                                sendf(sender_fd, err, strlen(err), MESSAGE);
                                break;
                            }

                            sprintf(tmp_buff, "You connected to room: %s\n", rooms[new_room].name);
                            sendf(sender_fd, tmp_buff, strlen(tmp_buff), MESSAGE);

                            sprintf(tmp_buff, "%s left", clients[sender_fd].name);

                            echo_room(tmp_buff, rooms, room_count, sender_fd, listener);

                            rooms[new_room].clients[sender_fd] = clients[sender_fd];
                            rooms[curr_room].clients[sender_fd] = null_usr;

                            sprintf(tmp_buff, "%s joined", clients[sender_fd].name);

                            echo_room(tmp_buff, rooms, room_count, sender_fd, listener);

                            // free(tmp_buff);
                            break;

                        default:
                            break;
                        }
                        free(tmp_buff);
                    }
                }
            }
        }
    }
    free(poll_list);

    // closing the listening socket
    shutdown(listener, SHUT_RDWR);
    close(listener);
    return 0;
}

void echo_all(const char *buffer, struct pollfd poll_list[ ], int fd_count, int sender, int listener) {
    for (int j = 0; j < fd_count; j++) {
        // Send to everyone!
        int dest_fd = poll_list[j].fd;

        // Except the listener and ourselves
        if (dest_fd != listener && dest_fd != sender) {
            if (sendf(dest_fd, buffer, strlen(buffer), MESSAGE) < 0) {
                perror("Failed to send");
            }
        }
    }
}

void echo_room(const char *buffer, room rooms[ ], int room_count, int sender, int listener) {
    room sender_room;
    for (int j = 0; j < room_count; j++) {
        if (rooms[j].clients[sender].socket == sender) {
            sender_room = rooms[j];
            break;
        }
    }

    for (int j = 0; j < MAX_CLIENTS; j++) {
        int dest_fd = sender_room.clients[j].socket;
        if (dest_fd != 0 && dest_fd != listener && dest_fd != sender) {
            if (sendf(dest_fd, buffer, strlen(buffer), MESSAGE) < 0) {
                perror("Failed to send");
            }
        }
    }
}

void poll_list_add(struct pollfd *poll_list[ ], int newfd, int *fd_count, int *fd_size) {
    // If we don't have room, add more space in the poll list array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it

        *poll_list = realloc(*poll_list, sizeof(**poll_list) * (*fd_size));
    }

    (*poll_list)[*fd_count].fd = newfd;
    (*poll_list)[*fd_count].events = POLLIN; // Check ready-to-read
    (*poll_list)[*fd_count].revents = 0;

    (*fd_count)++;
}

// Remove an index from the set
void poll_list_del(struct pollfd poll_list[ ], int i, int *fd_count) {
    // Copy the one from the end over this one
    poll_list[i] = poll_list[*fd_count - 1];

    (*fd_count)--;
}