#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

#define PORT_1 8877
#define PORT_2 8878

using namespace std;

void connect(int* out_socket, int port) {
    struct sockaddr_in addr = {0};
    
    // creating client socket
    if((*out_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket failure");
        exit(-1);
    }
    cout << "Socket created\n";

    // address params
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // connecting to server
    if((connect(*out_socket, (struct sockaddr*)&addr, sizeof(addr))) < 0) {
        perror("Connection failure");
        exit(-2);
    }
}

int main() {
    // creating arrs for sockets, ports
    int sockets[2];
    int server_num = -1;

    connect(&sockets[0], PORT_1);
    connect(&sockets[1], PORT_2);

    // command for sending, buffer for recieving
    string command;
    char buf[256];

    bool flag = true;
    // cycle for client work
    while(1) {
        if(server_num == -1) {
            // choosing server
            if(command != "0" && command != "1") {
                if(flag) {
                    cout << "Choose the server (0 | 1)" << endl;
                    cin >> command;
                    if(command == "0") {
                        server_num = 0;
                    } else if(command == "1") {
                        server_num = 1;
                    }
                    flag = false;
                } else {
                    cout << "Server " << command << " doesn't exist" << endl;
                    flag = true;
                    continue;
                }
            }
        } else {
            cout << "\nEnter a command (help):" << endl;
            cin >> command;

            if(command == "switch") {
                server_num = -1;
                flag = true;
                continue;
            } else if(command == "help") {
                cout << "Available commands: " << endl 
                    << "\thelp - get client commands (you're here)" << endl
                    << "\tshelp - get help for server commands" << endl
                    << "\tswitch - connect to other server" << endl;
            } else {
                const char *send_com = command.c_str();
                memset(buf, 0, 256);
                send(sockets[server_num], send_com, strlen(send_com), 0);
                recv(sockets[server_num], buf, sizeof(buf), 0);
                cout << buf << endl;
            }
        }
    }
    cout << "Client shutted down" << endl;
    return 0;
}