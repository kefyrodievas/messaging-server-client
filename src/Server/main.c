#include "server.h"

int main(int argc, char *argv[]) {
  char *port;
  if (argc > 1)
    // port = (uint16_t)strtoul(argv[1], NULL, 0);
    port = argv[1];
  else
    port = "8080";

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int listener = get_listener(port, &hints);

  if (listener < 0) {
    fprintf(stderr, "Failed to get listener");
    exit(EXIT_FAILURE);
  }

  server_start(listener);
}