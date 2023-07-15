#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <string>

using namespace std;

#define PORT 53300

int main() {
    int master_socket, addrlen, new_socket, client_socket[30],
        max_clients = 30, activity, valread, sd, max_sd;
    struct sockaddr_in address;

    fd_set readfds;

    char buffer[4096];
    char* names[30];

    for (int i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
        names[0] = { 0 };
    }

    // Create a socekt
    master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (master_socket < 0) {
        cout << "Unable to create a socket" << endl;
        return -1;
    }

    int opt = 1;
    setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    // Bind the socket to IP / port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    //inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    bind(master_socket, (sockaddr*)&address, sizeof(address));

    // Mark the socket for listening in
    listen(master_socket, SOMAXCONN);

    addrlen = sizeof(address);

    while (true) {
        FD_ZERO(&readfds);
        memset(buffer, 0, 4096);

        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        for (int i = 0; i < max_clients; i++) {

            int sd = client_socket[i];

            if (sd > 0) {
                // Accept a new connection
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);

        if (FD_ISSET(master_socket, &readfds)) {
            new_socket = accept(master_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);

            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n",
                new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            //valread = read(sd , buffer, 4096);

            //send new connection greeting message 
            string message = "Halo!\n\r";
            send(new_socket, message.c_str(), message.length() + 1, 0);

            //add new socket to array of sockets 
            for (int i = 0; i < max_clients; i++) {
                //if position is empty 
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    //names[i] = buffer;     
                    break;
                }
            }
        }

        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read(sd, buffer, 4096)) == 0) {
                    //Somebody disconnected , get his details and print 
                    getpeername(sd, (struct sockaddr*)&address, \
                        (socklen_t*)&addrlen);

                    //Close the socket and mark as 0 in list for reuse 
                    close(sd);
                    client_socket[i] = 0;
                    //names[i] = {0}; 
                }

                //Echo back the message that came in 
                else {
                    //set the string terminating NULL byte on the end 
                    //of the data read 
                    buffer[valread] = '\0';
                    for (int j = 0; j < max_clients; j++) {
                        int tmp_sd = client_socket[j];

                        if (sd == tmp_sd) continue;
                        if (tmp_sd == 0) continue;

                        send(tmp_sd, buffer, strlen(buffer), 0);
                    }
                }
            }
        }
    }

    return 0;
}
