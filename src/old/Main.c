#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 53300

char* join(const char* str1, const char* str2);
char* substr(char* string, int start, int count);
int find(char* string, char character);

int main(int argc, char** argv) {
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Creating socket file descriptor
    if (socket_fd < 0) {
        perror("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }
  
    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
  
    // Forcefully attaching socket to the port 8080
    // bind(listening, (struct sockaddr*)&addr, sizeof(addr));
    if (bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd, 3) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }


    socklen_t addrlen = sizeof(address);

    int new_socket, valread;
    char buffer[1024] = { 0 };
    char* hello = "Hello from server";
  

    if ((new_socket = accept(socket_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
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
    shutdown(socket_fd, SHUT_RDWR);
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