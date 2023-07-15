#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <thread>

#define PORT 53300

void receive(int sock);

int main(int argc, char** argv){

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        return -1;
    }
    std::string ipAddr;
    if(argc > 1)ipAddr = argv[1];
    else ipAddr = "127.0.0.1";

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    inet_pton(AF_INET, ipAddr.c_str(), &hint.sin_addr);
    
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if(connectRes == -1) {
        std::cout << "Server is offline\n";
        return -1;
    }
    std::cout << "\033c" << "Enter your username: ";
    std::string uname;
    std::string sendUname;
    std::cin >> uname;
    sendUname = "<name=" + uname + '>';
    send(sock, sendUname.c_str(), sendUname.size() + 1, 0);
    std::string userin;
    int sendres;

    std::thread (receive, sock).detach();

    while(true){
        // std::cout << uname << ": ";
        getline(std::cin, userin);

        if(userin != ""){
            sendres = send(sock, userin.c_str(), userin.size() + 1, 0);
            if(sendres == -1){
                std::cout << "Couldn't send to the server\r\n";
                continue;
            }
        }
    }
    close(sock);

}


void receive(int sock){
    int bytesRec;
    char buf[4096];
    while(true){
        memset(buf, 0, 4096);
        bytesRec = recv(sock, buf, 4096, 0);
        if(bytesRec == 0) {
            std::cout << "\nServer closed\n";
            break;
        }
        std::cout << std::string(buf, bytesRec) << "\r\n";
    }
}
