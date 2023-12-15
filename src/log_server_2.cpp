#include <unistd.h>
#include <ctime>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <signal.h>
#include <sys/stat.h>

using namespace std;


void _handler(int n) {
    cout << "Log server closing..." << endl;
    time_t cur = time(NULL);
    string msg = ctime(&cur);
    string msg_log = msg + "off";
    FILE *f = fopen("bin/logs/server_1_logs.txt", "a");
    fprintf(f, msg.c_str());
    fclose(f);
    exit(0);
}

void helper(string buf_msg) {
    time_t cur;
    string msg;
    cur = time(NULL);
    msg = ctime(&cur);
    msg += buf_msg + "\n";
    FILE *f = fopen("bin/logs/server_1_logs.txt", "a");
    fprintf(f, msg.c_str());
    fclose(f);
}

int main() {
    sleep(1);
    cout << "Log server started..." << endl;
    int log_fd = open("bin/log_2", O_RDONLY);
    if(log_fd == -1) {
        perror("Can't open FIFO");
        exit(-1);
    }
    helper("Log server started working...");
    char buf[1024];
    string buf_msg;
    while(1) {
        memset(buf, 0, 1024);
        sleep(1);
        read(log_fd, buf, sizeof(buf));
        if(buf[0] != 0) {
            cout << "Get log msg" << endl;
            buf_msg = buf;
            helper(buf_msg);
        }
        signal(SIGQUIT, _handler);
    }
    return 0;
}