#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../Lib/str.h"
#include "../msg.h"
#include "client.h"

#define PORT 8080

struct revc_thread_args {
  int socket_fd;
  int (*recv_callback)(char *);
};

void *receive(void *arg_arr);

int start_client(struct client_info info, int (*recv_callback)(char *)) {
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
  address.sin_port = htons(info.port);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, info.ip_address, &address.sin_addr) <= 0) {
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

  char uname[16];
  printf("Enter name: ");
  readline(stdin, uname, 16);

  int ret = sendf(socket_fd, uname, strlen(uname), NAME);
  if (ret < 0) {
    perror("Failed to send");
  }

  char buffer[BUFF_SIZE];
  char *cmd;

  struct revc_thread_args args;
  args.socket_fd = socket_fd;
  args.recv_callback = recv_callback;

  pthread_t receive_thread;
  pthread_create(&receive_thread, NULL, receive, (void *)&args);

  int loop = 1;
  while (loop) {
    readline(stdin, buffer, BUFF_SIZE);

    if (buffer[0] == '/') {
      cmd = substr(buffer, 1, 4);

      if (strcmp(cmd, "name") == 0) {
        char *name = substr(buffer, 5, strlen(buffer));

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
        free(name);
        continue;
      }

      if (strcmp(cmd, "room") == 0) {
        char *room = substr(buffer, 5, strlen(buffer));

        int ret = sendf(socket_fd, room, strlen(room), ROOM);
        if (ret < 0) {
          perror("Failed to send");
        }
        free(room);
        continue;
      }

      if (strcmp(cmd, "exit") == 0) {
        break;
      }
    }

    int ret = sendf(socket_fd, buffer, strlen(buffer), MESSAGE);
    if (ret < 0) {
      perror("Failed to send");
    }
  }
  free(cmd);

  pthread_cancel(receive_thread);
  pthread_join(receive_thread, NULL);

  // Close the connected socket
  close(socket_fd);
  return 0;
}

void *receive(void *arg_arr) {
  struct revc_thread_args args = *(struct revc_thread_args *)arg_arr;
  char buffer[BUFF_SIZE];

  while (1) {
    memset(buffer, 0, BUFF_SIZE);
    char type;
    int bytesRecv = recvf(args.socket_fd, buffer, &type);
    // int bytesRecv = recv(socket_fd, buffer, BUFF_SIZE, 0);

    if (bytesRecv < 0) {
      perror("Connection issue");
      break;
    }

    if (bytesRecv == 0) {
      perror("Server disconnected");
      break;
    }

    (*args.recv_callback)(buffer);

    // Display message
    // printf("%s\n", buffer);
  }
  return NULL;
}