#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <arpa/inet.h>
#include <sys/socket.h>

// #include "str.h"

#define BUFF_SIZE 4096
#define PORT 8080
#define ADDR "127.0.0.1"

int client_start( );

#endif