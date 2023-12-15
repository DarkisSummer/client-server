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
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#define PORT 8877

using namespace std;

int log_fd, serv_sock;
pid_t log_pid;

void log_send(string msg) {
    log_fd = open("bin/log_1", O_WRONLY);
    write(log_fd, msg.c_str(), strlen(msg.c_str()));
    close(log_fd);
}

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
    msg = "\nsent msg to client:\n" + msg;
    cout << msg << endl;
    log_send(msg);
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
    msg = "\nsent msg to client:\n" + msg;
    cout << msg << endl;
    log_send(msg);
}

void server_func(int fd) {
    char buff[256];
    // cycle for server work
    string command, msg, log_msg;
    cout << "Server started working..." << endl;
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
            "\tmonsize - recieve width x height of main screen\n"
            "\toff - turn off the server";
            send(fd, msg.c_str(), strlen(msg.c_str()), 0);
            cout << "sent msg to client:\n" << msg << endl;   
        } else if(command == "monnum") {
            log_msg = "Got command:" + command;
            log_send(log_msg);
            command_1(fd);
            continue;
        } else if(command == "monsize") {
            log_msg = "Got command:" + command;
            log_send(log_msg);
            command_2(fd);
            continue;
        } else if(command == "off") {
            kill(log_pid, SIGQUIT);
            sleep(1);
            close(log_fd);
            system("rm bin/log_1");
            close(serv_sock);
            break;
        } 
        else {
            msg = "Invalid command, try shelp";
            send(fd, msg.c_str(), strlen(msg.c_str()), 0);
            cout << "sent msg to client:\n" << msg << endl;
            log_msg = "Got command:" + command + "\nsent msg to client:\n" + msg;
            log_send(log_msg);
            continue;
        }
    }
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
    cout << "Binding success..." << endl;

    // defining num of listeners
    if((listen(serv_sock, 1)) < 0) {
        perror("Listen failure");
        exit(-3);
    }
    cout << "Listening success..." << endl;

    system("rm bin/log_1");
    // creating fifo for log server (additional task)
    if(mkfifo("bin/log_1", 0777) == -1) {
        perror("FIFO failure");
        exit(-5);
    }
    cout << "Creating FIFO success" << endl;
    // creating child process for log server
    pid_t log_pid = fork();
    if(log_pid == 0) {
        cout << "Child started working..." << endl;
        execl("bin/log_server_1", NULL);
    }

    // Accepting client
    int fd;
    pid_t pid;
    while(1) {
        if((fd = accept(serv_sock, NULL, NULL)) < 0) {
            perror("Accept failure");
            exit(-4);
        } else {
            cout << "Accepting success..." << endl;
            pid = fork();
            if(pid == 0) {
                server_func(fd);
            } else {
                close(fd);
            }
        }
    }

    return 0;
}