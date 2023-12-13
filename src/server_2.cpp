#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <ctime>

#define PORT 8878

using namespace std;

string task2_cases[] {"b", "k", "m", "g"};


void log_send(int fd, string msg) {
    time_t cur = time(NULL);
}

void command_1(int fd) {
    string msg {"OS version: "};
    // getting info about OS verion
    FILE *f = popen("lsb_release -d", "r");
    char buf[128];
    memset(buf, 0, 128);
    if(fgets(buf, sizeof(buf), f) != NULL) {
        string s {buf};
        msg += s.substr(s.find_first_of("\t"));
    }
    pclose(f);
    send(fd, msg.c_str(), strlen(msg.c_str()), 0);
    cout << "sent msg to client:\n" << msg << endl;
}


int command_2(int fd) {
    // getting info about memory
    send(fd, "Input units:", sizeof("Input units:"), 0);
    char buf[10];
    recv(fd, buf, sizeof(buf), 0);
    string buff = buf;
    bool f = false;
    for(auto s: task2_cases) if (s == buff) {f = true; break;}
    if(!f) {send(fd, "Invalid units\n", sizeof("Invalid units\n"), 0); return 1;}
    string cmd = "free -" + buff + " > ../bin/free";
    system(cmd.c_str());
    ifstream ffile ("../bin/free");
    string msg {"Free phys memory: "};
    int count {0};
    string line;
    if(ffile.is_open()) {
        while(getline(ffile, line)) {
            count++;
            if(count == 2) {
                count = 0;
                for(int i=0; i<line.length(); i++) {
                    if((line[i] == ' ') && (line[i+1] != ' ')) {
                        count++;
                    }
                    if(count == 3 && line[i] != ' ') {
                        msg += line[i];
                    }
                }
                break;
            } else {
                continue;
            }
        }
    }
    ffile.close();
    /*
    if(ffile.is_open()) {
        while(getline(ffile, line)) {
            count++;
            if(count == 2) {
                msg += line.substr(line.find_last_of(" ")+1);
                break;
            }
        }
    }
    */
    ffile.close();
    system("rm ../bin/free");
    // sending ans to client
    send(fd, msg.c_str(), strlen(msg.c_str()), 0);
    cout << "sent msg to client:\n" << msg << endl;
    return 0;
}


int main() {
    // define server socket, address
    int serv_sock;
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
    if(((fd = accept(serv_sock, NULL, NULL))) < 0) {
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
            msg = "Server 2 commands: \n"
            "\tshelp - get server commands (you're here)\n"
            "\tosver - recieve current version of OS\n"
            "\tfreemem - recieve amount of free physical memory (using requested units)";
            send(fd, msg.c_str(), strlen(msg.c_str()), 0);
            cout << "sent msg to client:\n" << msg << endl;
            continue;
        } else if(command == "osver") {
            command_1(fd);
            continue;
        } else if(command == "freemem") {
            command_2(fd);
            sleep(1);
            continue;
        } else {
            msg = "Invalid command, try shelp";
            send(fd, msg.c_str(), strlen(msg.c_str()), 0);
            cout << "sent msg to client:\n" << msg << endl;
            continue;
        }
    }

    close(serv_sock);
    return 0;
}