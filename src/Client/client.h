#ifndef _CLIENT_H
#define _CLIENT_H

#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

struct client_info {
  char *ip_address;
  uint16_t port;
};

int start_client(struct client_info info, int (*recv_callback)(char *));

#endif // _CLIENT_H
