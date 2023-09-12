#ifndef _SERVER_H
#define _SERVER_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int get_listener(char *port, struct addrinfo *hints);
extern int server_start(int listener);

#endif // _SERVER_H