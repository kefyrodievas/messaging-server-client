#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Lib/str.h"
#include "../Lib/vector.h"
#include "../msg.h"
#include "server.h"

// #define PORT 8080
#define MAX_CLIENTS 30

typedef struct {
  int socket;
  char *name;
  char address[INET6_ADDRSTRLEN];
} client;

typedef struct {
  char *name;
  int client_count;
  client clients[MAX_CLIENTS];
} room;

void echo_all(const char *buffer, vector_t poll_list, int sender, int listener);
void echo_room(const char *buffer, room rooms[], int room_count, int sender,
               int listener);

int server_start(int listener) {
  struct sockaddr_storage address;

  client null_usr = {.socket = 0, .name = NULL, .address = {0}};

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
  for (room_count = 0; readline(rooms_file, room_name, 16) != EOF;
       room_count++) {
    printf("%s\n", room_name);
    rooms[room_count].name = strdup(room_name);
    rooms[room_count].client_count = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
      rooms[room_count].clients[i] = null_usr;
    }
  }
  fclose(rooms_file);

  socklen_t addrlen = sizeof(address);

  // char *buffer = (char*)malloc(BUFF_SIZE * sizeof(char));
  // memset(buffer, 0, sizeof(buffer));

  char buffer[BUFF_SIZE];

  vector_t poll_list = vector(struct pollfd);

  struct pollfd pfd;
  pfd.fd = listener;
  pfd.events = POLLIN;
  vector_push(poll_list, pfd);

  while (1) {
    int events = poll((struct pollfd *)poll_list.data, poll_list.len, -1);

    if (events < 0) {
      fprintf(stderr, "Poll failed");
      exit(EXIT_FAILURE);
    }

    if (events == 0) {
      printf("Poll timed out!\n");
      break;
    }

    for (size_t i = 0; i < poll_list.len; i++) {

      // Check if someone's ready to read
      pfd = vector_get(poll_list, i, struct pollfd);
      if (pfd.revents & POLLIN) { // We got one!!

        if (pfd.fd == listener) {
          // If listener is ready to read, handle new connection

          int socket = accept(listener, (struct sockaddr *)&address,
                              (socklen_t *)&addrlen);
          if (socket < 0) {
            perror("Accept failed");
            continue;
          }

          clients[socket].socket = socket;
          clients[socket].name = NULL;

          inet_ntop(address.ss_family, get_in_addr((struct sockaddr *)&address),
                    clients[socket].address, INET6_ADDRSTRLEN);

          rooms[0].clients[socket] = clients[socket];
          rooms[0].client_count++;

          printf("New connection, socket: %d IP: %s\n", socket,
                 clients[socket].address);

          char *tmp_buff = malloc(BUFF_SIZE);
          sprintf(tmp_buff, "Hello. You are connected to room: %s\n",
                  rooms[0].name);

          sendf(socket, tmp_buff, strlen(tmp_buff), MESSAGE);

          free(tmp_buff);

          struct pollfd tmp_pfd;
          tmp_pfd.fd = socket;
          tmp_pfd.events = POLLIN;
          vector_push(poll_list, tmp_pfd);

        } else {
          int sender_fd = pfd.fd;

          // If not the listener, we're just a regular client
          char type;
          ssize_t nbytes = recvf(sender_fd, buffer, &type);

          printf("%s\n", buffer);

          if (nbytes <= 0) {
            // When client disconnects
            if (nbytes == 0) {
              char *tmp_buff = malloc(BUFF_SIZE);
              sprintf(tmp_buff, "%s disconnected", clients[sender_fd].name);

              echo_all(tmp_buff, poll_list, sender_fd, listener);

              free(tmp_buff);
            } else {
              perror("Receive failed");
            }

            clients[sender_fd] = null_usr;

            for (int j = 0; j < room_count; j++) {
              if (rooms[j].clients[sender_fd].socket == sender_fd) {
                rooms[j].clients[sender_fd] = null_usr;
                rooms[j].client_count--;
                break;
              }
            }

            close(sender_fd);
            vector_remove(poll_list, i);
          } else {
            char *tmp_buff = malloc(BUFF_SIZE + strlen(buffer));

            switch (type) {
            case MESSAGE:
              sprintf(tmp_buff, "%s: %s", clients[sender_fd].name, buffer);

              echo_room(tmp_buff, rooms, room_count, sender_fd, listener);

              break;

            case NAME:
              if (clients[sender_fd].name == NULL) {
                clients[sender_fd].name = strdup(buffer);

                sprintf(tmp_buff, "%s connected", clients[sender_fd].name);
                echo_all(tmp_buff, poll_list, sender_fd, listener);
              } else {
                sprintf(tmp_buff, "%s is now known as %s\n",
                        clients[sender_fd].name, buffer);

                echo_room(tmp_buff, rooms, room_count, sender_fd, listener);

                clients[sender_fd].name = strdup(buffer);
              }
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
                char err[] = "Room does not exist\n";
                sendf(sender_fd, err, strlen(err), MESSAGE);
                break;
              }
              if (curr_room == new_room) {
                char err[] = "You are already connected to this room\n";
                sendf(sender_fd, err, strlen(err), MESSAGE);
                break;
              }

              sprintf(tmp_buff, "You connected to room: %s\n",
                      rooms[new_room].name);
              sendf(sender_fd, tmp_buff, strlen(tmp_buff), MESSAGE);

              sprintf(tmp_buff, "%s left", clients[sender_fd].name);

              echo_room(tmp_buff, rooms, room_count, sender_fd, listener);

              rooms[new_room].clients[sender_fd] = clients[sender_fd];
              rooms[curr_room].clients[sender_fd] = null_usr;

              sprintf(tmp_buff, "%s joined", clients[sender_fd].name);

              echo_room(tmp_buff, rooms, room_count, sender_fd, listener);

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

  for (int i = 0; i < room_count; i++) {
    free(rooms[i].name);
    for (int j = 0; j < rooms[i].client_count; j++) {
      free(rooms[i].clients[j].name);
      free(rooms[i].clients[j].address);
    }
  }
  for (int i = 0; i < MAX_CLIENTS; i++) {
    free(clients[i].address);
    free(clients[i].name);
  }

  vector_destroy(poll_list);

  // closing the listening socket
  shutdown(listener, SHUT_RDWR);
  close(listener);
  return 0;
}

void echo_all(const char *buffer, vector_t poll_list, int sender,
              int listener) {
  for (size_t i = 0; i < poll_list.len; i++) {
    // Send to everyone!
    struct pollfd pfd = vector_get(poll_list, i, struct pollfd);
    int dest_fd = pfd.fd;

    // Except the listener and ourselves
    if (dest_fd != listener && dest_fd != sender) {
      if (sendf(dest_fd, buffer, strlen(buffer), MESSAGE) < 0) {
        fprintf(stderr, "Failed to send");
      }
    }
  }
}

void echo_room(const char *buffer, room rooms[], int room_count, int sender,
               int listener) {
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
        fprintf(stderr, "Failed to send");
      }
    }
  }
}

int get_listener(char *port, struct addrinfo *hints) {
  int listener;
  int opt = 1;
  int rv;

  struct addrinfo *ai, *p;

  // char *port_str = NULL;
  // sprintf(port_str, "%u", port);

  if ((rv = getaddrinfo(NULL, port, hints, &ai)) != 0) {
    fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
    exit(1);
  }

  for (p = ai; p != NULL; p = p->ai_next) {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) {
      continue;
    }

    // Lose the pesky "address already in use" error message
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
      close(listener);
      continue;
    }

    break;
  }

  freeaddrinfo(ai);

  // If we got here, it means we didn't get bound
  if (p == NULL) {
    fprintf(stderr, "Socket creation failed\n");
    return -1;
  }

  // Listen
  if (listen(listener, 10) == -1) {
    fprintf(stderr, "Listening failed\n");
    return -1;
  }

  return listener;
}