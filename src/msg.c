#include "server.h"
#include "client.h"

int main(int argc, char *argv[ ]) {
    if (argc < 2) return 0;

    if (argv[1] == "server") {
        printf("Starting server");
        server_start( );
    }
    if (argv[1] == "client") {
        printf("Starting client");
        client_start( );
    }

    return 0;
}