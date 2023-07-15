#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>

#define PORT 8080
#define MAX_CLIENTS 30

char* join(const char* str1, const char* str2);
char* substr(char* string, int start, int count);
int find(char* string, char character);

typedef struct user{
    int socket;
    char* name;
    char* address;
} user;




int main(int argc, char** argv) {
    char buffer[1024] = { 0 };
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    fd_set readfds;
    int max_socket, temp_socket;

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
    //address.sin_addr
    // inet_ntoa(address.sin_addr);

    // Forcefully attaching socket to the port 8080
    // bind(listening, (struct sockaddr*)&addr, sizeof(addr));
    if (bind(master_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(master_socket, MAX_CLIENTS) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client[i].socket = 0;
    }
    socklen_t addrlen = sizeof(address);

    int new_socket, valread;
    
    char* hello = "Hello from server";
  
    while(1){
        memset(buffer, 0, 1024);
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_socket = master_socket;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if(client[i].socket > 0){
                FD_SET(client[i].socket, &readfds);
            }
            if(client[i].socket > max_socket){
                max_socket = client[i].socket;
            }
        }
        select(max_socket + 1, &readfds, NULL, NULL, NULL);
        if(FD_ISSET(master_socket, &readfds)){
            temp_socket = accept(master_socket, (struct sockaddr*)&address, (socklen_t *)addrlen);
            printf("new connection\n");
            send(temp_socket, hello, strlen(hello), 0);

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client[i].socket == 0) {
                    client[i].socket = temp_socket;
                    client[i].address = inet_ntoa(address.sin_addr);
                    break;
                }
            }
        }
    }
    if ((new_socket = accept(master_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
  
    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(master_socket, SHUT_RDWR);
    return 0;
}

char* join(const char* str1, const char* str2) {
    char* ret = malloc(strlen(str1) + strlen(str2));//4096 * sizeof(char));
    strcpy(ret, str1);
    strcat(ret, str2);
    return ret;
}

char* substr(char* string, int start, int end){
    int length = end - start;
    char* ret = malloc(length * sizeof(char));
    for(int i = 0; i < length; i++){
        ret[i] = string[i + start];
    }
    return ret;
}

int find(char* string, char character){
    for(int i = 0; i < strlen(string); i++){
        if(character == string[i]) return i;
    }
    return -1;
}