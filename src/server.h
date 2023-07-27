#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>

// #include "str.h"

#define BUFF_SIZE 4096
#define PORT 8080
#define MAX_CLIENTS 30



int server_start( );

#endif