#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pty.h>



int main() {
    int master;
    char* slave_name;
    
    pid_t pid = forkpty(&master, slave_name, NULL, NULL);
    
    if (pid == 0) {
        execlp("/usr/bin/bash", "/usr/bin/bash", NULL);
    }
    
    return 0;
}
