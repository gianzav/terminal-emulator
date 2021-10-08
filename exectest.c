#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>


int main() {

    pid_t pid = fork();

    int _stdout = dup(STDOUT_FILENO);

    if (pid == 0) {

        dup2(_stdout, STDOUT_FILENO);
        int err = execlp("/usr/bin/head", "/usr/bin/head", "/etc/services", (char*) NULL);

        if (err == -1) {
            perror("error: ");
        }
    }

    return 0;
}
