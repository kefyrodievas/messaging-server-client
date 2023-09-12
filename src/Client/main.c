#include "client.h"
#include <stdio.h>

int recv_callback(char *buff) { return printf("%s\n", buff); }

int main(int argc, char *argv[]) {
  char *ip_address;
  if (argc > 1)
    ip_address = argv[1];
  else
    ip_address = "127.0.0.1";

  struct client_info info;
  info.ip_address = ip_address;
  info.port = 8080;

  start_client(info, &recv_callback);
}