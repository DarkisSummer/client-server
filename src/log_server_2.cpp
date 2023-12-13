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

ofstream ff;

void _handler(int n) {
    time_t cur = time(NULL);
    string msg = ctime(&cur);
    string msg_log = msg + " off";
    ff << msg_log;
    ff.close();
    exit(0);
}

int main() {
    sleep(1);
    cout << "Log server started" << endl;
    int log_fd = open("log_2", O_RDONLY);
    if(log_fd = -1) {
        perror("Can't open FIFO");
        exit(-1);
    }

    ff.open("../bin/logs/server_2_logs.txt");

    char buf[1024];
    string msg, buf_msg;
    time_t cur;
    while(1) {
        memset(buf, 0, 1024);
        read(log_fd, buf, sizeof(buf));
        if(buf[0] != 0) {
            cur = time(NULL);
            msg = ctime(&cur);
            buf_msg = buf;
            msg += buf_msg + "\n";
            ff << msg;
        }
        signal(SIGQUIT, _handler);
    }
    return 0;
}