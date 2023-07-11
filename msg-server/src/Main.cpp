#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <fstream>


#define PORT 53300
#define MAX_CLIENTS 30

struct user {
    int socket;
    std::string name;
    char* address;
    int channelID;
};

struct channel{
    std::string name;
    int id;
};

std::string* roomRead(char* filename, int &n);
char* join(const char* str1, const char* str2);

std::ifstream input;
std::ofstream output;

int main() {
    int channelCount;
    std::string* channelNames = roomRead("channels", channelCount);
    for(int i = 0; i < channelCount; i++){
        std::cout << channelNames[i] << "\n";
    }
    // create a socket
    int sock_desc, max_sock, read_value, temp_sock, tmp;
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    char* buf = (char*)malloc(4096 * sizeof(char));
    user client[MAX_CLIENTS];
    fd_set readfds;
    if (listening <= 0) {
        std::cerr << "Can't create a socket";
        return -1;
    }

    // bind socket to IP/Port
    sockaddr_in addr;
    addr.sin_family = AF_INET;

    addr.sin_port = htons(PORT); //htons is necessary here
    std::cout << ntohs(addr.sin_port) << std::endl;
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr); //converts a string to an array of numbers

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client[i].socket = 0;
    }

    bind(listening, (sockaddr*)&addr, sizeof(addr));
    // mark the socket for listening

    if (listen(listening, SOMAXCONN) == -1) {
        std::cerr << "Can't listen";
        return -3;
    }

    socklen_t addrlen = sizeof(addr);

    while (1) {
        memset(buf, 0, 4096);
        FD_ZERO(&readfds);

        FD_SET(listening, &readfds);
        max_sock = listening;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sock_desc = client[i].socket;
            if (sock_desc > 0) {

                FD_SET(sock_desc, &readfds);
            }
            if (sock_desc > max_sock) {
                max_sock = sock_desc;
            }
        }

        select(max_sock + 1, &readfds, nullptr, nullptr, nullptr);
        if (FD_ISSET(listening, &readfds)) {
            temp_sock = accept(listening, (sockaddr*)&addr, (socklen_t*)&addrlen);
            std::cout << "New connection, socket: " << temp_sock << ", IP: " << inet_ntoa(addr.sin_addr) << "\n";

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client[i].socket == 0) {
                    client[i].socket = temp_sock;
                    client[i].address = inet_ntoa(addr.sin_addr);
                    break;
                }
            }

        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            sock_desc = client[i].socket; //implement threading with lambda expressions
            if (FD_ISSET(sock_desc, &readfds)) {
                read_value = read(sock_desc, buf, 4096);

                if (read_value == 0) {
                    close(sock_desc);

                    buf[read_value] = '\0';
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (sock_desc == client[j].socket) continue;
                        if (client[j].socket == 0) continue;
                        buf = join((client[i].name + " -> ").c_str(), "Disconnected");
                        send(client[j].socket, buf, strlen(buf), 0);
                    }

                    client[i].socket = 0;
                    client[i].name = "";
                }
                else if (std::string(buf).substr(0, 6) == "<name=") { // reads the username that is sent upon starting the client
                    client[i].name = std::string(buf).substr(6, std::string(buf).find(">"));
                    client[i].name.erase(client[i].name.find(">"));

                    buf[read_value] = '\0';
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (sock_desc == client[j].socket) continue;
                        if (client[j].socket == 0) continue;
                        buf = join((client[i].name + " -> ").c_str(), "Connected");
                        send(client[j].socket, buf, strlen(buf), 0);
                    }
                }
                else {
                    buf[read_value] = '\0';
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (sock_desc == client[j].socket) continue;
                        if (client[j].socket == 0) continue;
                        buf = join((client[i].name + ": ").c_str(), (const char*)buf);
                        send(client[j].socket, buf, strlen(buf), 0);
                    }
                }
            }
        }
    }


    return 0;
}

char* join(const char* str1, const char* str2) {
    char* ret = new char[4096];
    strcpy(ret, str1);
    strcat(ret, str2);
    return ret;
}
// change it to an int function and make it retun error codes
std::string * roomRead(char* filename, int &n){
    std::string tmp;
    int i = 0;
    input.open(filename);
    while(input >> tmp){
        i++;
    }
    std::string *ret;
    ret = (std::string*)malloc(i * sizeof(std::string));
    input.close();
    input.open(filename);

    for(int j = 0; j < i; j++){
        input >> tmp;
        ret[j] = tmp;
    }
    n = i;
    return ret;
}
