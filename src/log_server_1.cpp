#include <unistd.h>
#include <ctime>
#include <string>
#include <fcntl.h>

#include <errno.h>
#include <signal.h>
#include <sys/stat.h>

using namespace std;

ofstream ff;

int main() {
    ff.open("../bin/logs/server_1_log");

}