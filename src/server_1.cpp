#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <regex>

#define PORT 8877

using namespace std;

void command_1(int fd) { // fucntion that send num of monitors to client
    string msg {"Number of screens: "};
    // getting info about screens
    system("xdpyinfo > bin/xdpyinfo");
    ifstream ffile ("bin/xdpyinfo");
    
    // parsing file until we find needed line
    string line;
    if(ffile.is_open()) {
        while(getline(ffile, line)) {
            if((line.rfind("number of screens", 0) == 0)) {
                msg += line[line.length()-1];
                break;
            }
        }
    }
    ffile.close();
    system("rm bin/xdpyinfo");
    // sending ans to client
    send(fd, msg.c_str(), strlen(msg.c_str()), 0);
    cout << "sent msg to client:\n" << msg << endl;
}

void command_2(int fd) {
    string msg {"Default screen: "};
    // getting info about screens
    system("xdpyinfo > bin/xdpyinfo");
    ifstream ffile ("bin/xdpyinfo");
    
    // parsing file until we find needed line
    string line;
    if(ffile.is_open()) {
        while(getline(ffile, line)) {
            if((line.rfind("default screen", 0) == 0)) {
                msg += line[line.length()-1];
            }
            if((line.rfind("  dimensions:", 0) == 0)) {
                msg += "\nIt's dimensions: ";
                msg += line.substr(strlen("dimensions:")+6, line.length()-strlen("dimensions:")+6);
            }
        }
    }
    ffile.close();
    system("rm bin/xdpyinfo");
    // sending ans to client
    send(fd, msg.c_str(), strlen(msg.c_str()), 0);
    cout << "sent msg to client:\n" << msg << endl;
}

int main() {
    // define server socket, address
    int serv_sock {0};
    if((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket failure");
        exit(-1);
    }
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // binding socket to addr
    if((bind(serv_sock, (struct sockaddr*)&addr, sizeof(addr))) < 0) {
        perror("Binding failure");
        exit(-2);
    }

    // defining num of listeners
    if((listen(serv_sock, 1)) < 0) {
        perror("Listen failure");
        exit(-3);
    }

    // Accepting client
    int fd;
    if((fd = accept(serv_sock, NULL, NULL)) < 0) {
        perror("Accept failure");
        exit(-4);
    }

    char buff[256];
    // cycle for server work
    string command, msg;
    while(1) {
        // recieving command from client
        memset(buff, 0, 256);
        recv(fd, buff, sizeof(buff), 0);
        command = buff;

        cout << "\nGot command: " << command << endl;

        // command cases
        if(command == "shelp") {
            msg = "Server 1 commands: \n"
            "\tshelp - get server commands (you're here)\n"
            "\tmonnum - recieve number of screens\n"
            "\tmonsize - recieve width x height of main screen";
            send(fd, msg.c_str(), strlen(msg.c_str()), 0);
            cout << "sent msg to client:\n" << msg << endl;   
        } else if(command == "monnum") {
            command_1(fd);
            continue;
        } else if(command == "monsize") {
            command_2(fd);
            continue;
        } else {
            msg = "Invalid command, try shelp";
            send(fd, msg.c_str(), strlen(msg.c_str()), 0);
            cout << "sent msg to client:\n" << msg << endl;   
        }
    }

    close(serv_sock);
    return 0;
}