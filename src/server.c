#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>

#include "str.h"

#define BUFF_SIZE 4096
#define PORT 8080
#define MAX_CLIENTS 30

typedef struct {
    int socket;
    char *name;
    char *address;
} user;


int main(int argc, char **argv) {
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    // Creating socket file descriptor
    if (master_socket < 0) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    user client[MAX_CLIENTS];
    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(master_socket, MAX_CLIENTS) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client[i].socket = 0;
        client[i].name = "";
        client[i].address = "";
    }

    socklen_t addrlen = sizeof(address);

    char *buffer = (char*)malloc(BUFF_SIZE * sizeof(char));
    // memset(buffer, 0, sizeof(buffer));

    // char buffer[BUFF_SIZE];

    char *hello = "Hello from server";
    // char * a = malloc(1);
    // a[2] = 'a';
    int max_socket;
    fd_set readfds;

    while (1) {
        // memset(buffer, 0, BUFF_SIZE * sizeof(char));
        // buffer[0] = '\0';
        buffer = "";
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_socket = master_socket;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int socket = client[i].socket;
            if (socket > 0) { FD_SET(socket, &readfds); }
            if (socket > max_socket) { max_socket = socket; }
        }
        select(max_socket + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(master_socket, &readfds)) {
            int socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
            if (socket < 0) {
                perror("Accept failed");
                continue;
            }
            printf("New connection, socket: %d IP: %s\n", socket, inet_ntoa(address.sin_addr));
            send(socket, hello, strlen(hello), 0);

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client[i].socket == 0) {
                    client[i].socket = socket;
                    client[i].address = inet_ntoa(address.sin_addr);
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            int socket = client[i].socket;
            if (!FD_ISSET(socket, &readfds)) { continue; }
            int read_value = recv(socket, buffer, BUFF_SIZE, 0);
            printf("%s\n", buffer);
            if (read_value < 0) {
                perror("Receive failed");
                continue;
            }

            // When client disconnects
            else if (read_value == 0) {
                buffer[read_value] = '\0';
                close(socket);
                for (int j = 0; j < MAX_CLIENTS; j++) {
                    if (socket == client[j].socket) continue;
                    if (client[j].socket == 0) continue;
                    send(client[j].socket, "Disconnected", 13, 0);
                }
                
                client[i].socket = 0;
                client[i].address = NULL;
            }
            // else if (std::string(buf).substr(0, 6) == "<name=") { // reads the username that is sent upon starting the client
            //     client[i].name = std::string(buf).substr(6, std::string(buf).find(">"));
            //     client[i].name.erase(client[i].name.find(">"));

            //     buf[read_value] = '\0';
            //     for (int j = 0; j < MAX_CLIENTS; j++) {
            //         if (socket == client[j].socket) continue;
            //         if (client[j].socket == 0) continue;
            //         buf = join((client[i].name + " -> ").c_str(), "Connected");
            //         send(client[j].socket, buf, strlen(buf), 0);
            //     }
            // }
            
            else {
                buffer[read_value] = '\0';
                for (int j = 0; j < MAX_CLIENTS; j++) {
                    if (socket == client[j].socket) continue;
                    if (client[j].socket == 0) continue;
                    // buf = join((client[i].name + ": ").c_str(), (const char*)buf);
                    send(client[j].socket, buffer, strlen(buffer), 0);
                }
            }
        }
    }
    

    // closing the listening socket
    shutdown(master_socket, SHUT_RDWR);
    return 0;
}