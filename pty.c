#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <fcntl.h>
#include <error.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/select.h>

#define BUF_SIZE 256
#define MAX_SNAME 1000

struct termios ttyOrig;


int ptyMasterOpen(char* slaveName, size_t snLen) {

    int master;
    char* p;

    /* Open an unused pty master device */
    master = posix_openpt(O_RDWR | O_NOCTTY);

    if (master == -1) {
        perror("error: ");
        return -1;
    }

    /* Change the ownership of the slave device */
    if (grantpt(master) == -1) {
        close(master);
        perror("error: ");
        return -1;
    }

    /* Unlock the pty slave so then it can be opened */
    /* The lock is necessary so the slave can be initialized before it being */
    /* opened by a process */
    if (unlockpt(master) == -1) {
        close(master);
        perror("error: ");
        return -1;
    }

    /* Get the name of the slave device corresponding to master */
    p = ptsname(master);

    if (p == NULL) {
        close(master);
        perror("error: ");
        return -1;
    }

    if (strlen(p) < snLen) {
        strncpy(slaveName, p, snLen);
    } else {
        close(master);
        errno = EOVERFLOW;
        return -1;
    }

    return master;
}


pid_t ptyFork(int* masterFd, char* slaveName, size_t snLen,
    const struct termios* slaveTermios, const struct winsize* slaveWS) {

    int mfd, slaveFd, savedErrno;
    pid_t childPid;
    char slname[MAX_SNAME];

    mfd = ptyMasterOpen(slname, MAX_SNAME);
    if (mfd == -1)
    return -1;

    if (slaveName != NULL) {
        if (strlen(slname) < snLen) {
            strncpy(slaveName, slname, snLen);
        } else {
            close(mfd);
            errno = EOVERFLOW;
            return -1;
        }
    }

    childPid = fork();

    if (childPid == -1) {
        savedErrno = errno;
        close(mfd);
        errno = savedErrno;
        return -1;
    }

    if (childPid != 0) {
        *masterFd = mfd;
        return childPid;
    }

    if (setsid() == -1) {
        perror("ptyFork:setsid");
    }

    close(mfd);

    slaveFd = open(slname, O_RDWR);
    if (slaveFd == -1) {
        perror("ptyFork:open-slave");
    }

#ifdef TIOCSCTTY
    if (ioctl(slaveFd, TIOCSCTTY, 0) == -1)
    perror("ptyFork:ioctl-TIOCSCTTY");
#endif

    if (slaveTermios != NULL) {
        if (tcsetattr(slaveFd, TCSANOW, slaveTermios) == -1)
            perror("ptyFork:tcsetattr");
    }

    if (slaveWS != NULL) {
        if (ioctl(slaveFd, TIOCSWINSZ, slaveWS) == -1)
        perror("ptyFork:ioctl-TIOCSWINSZ");
    }

    if (dup2(slaveFd, STDIN_FILENO) != STDIN_FILENO)
        perror("ptyFork:dup2-STDIN_FILENO");
    if (dup2(slaveFd, STDOUT_FILENO) != STDOUT_FILENO)
        perror("ptyFork:dup2-STDOUT_FILENO");
    if (dup2(slaveFd, STDERR_FILENO) != STDERR_FILENO)
        perror("ptyFork:dup2-STDERR_FILENO");

    if (slaveFd > STDERR_FILENO)
        close(slaveFd);

    return 0;
}

int main() {

    char slaveName[MAX_SNAME];
    char* shell;
    int masterFd;
    struct winsize ws;
    fd_set inFds;
    char buf[BUF_SIZE];
    ssize_t numRead;
    pid_t childPid;


    /* Retrieve the attributes and window size of the terminal under which the */
    /* program is run */
    if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1)
        perror("tcgetattr");
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
        perror("ioctl-TIOCSWINSZ");

    childPid = ptyFork(&masterFd, slaveName, MAX_SNAME, &ttyOrig, &ws);
    if (childPid == -1)
        perror("ptyFork");

    /* Execute a shell based on the SHELL env variable */
    if (childPid == 0) {
        shell = getenv("SHELL");

        /* The env variable may not be set */
        if (shell == NULL || *shell == '\0')
            shell = "/bin/sh";

        execlp(shell, shell, (char*) NULL);

        /* Should't get here unless there's an error */
        perror("execlp");
    }

    for (;;) {
        FD_ZERO(&inFds);
        FD_SET(STDIN_FILENO, &inFds);
        FD_SET(masterFd, &inFds);

        if (select(masterFd + 1, &inFds, NULL, NULL, NULL) == -1)
            perror("select");

        if (FD_ISSET(STDIN_FILENO, &inFds)) {
            numRead = read(STDIN_FILENO, buf, BUF_SIZE);
            if (numRead <= 0)
                exit(EXIT_SUCCESS);

            if (write(masterFd, buf, numRead) != numRead) {
                perror("FATAL partial/failed write (masterFd)");
                exit(EXIT_FAILURE);
            }
        }

        if (FD_ISSET(masterFd, &inFds)) {
            numRead = read(masterFd, buf, BUF_SIZE);
            if (numRead <= 0)
                exit(EXIT_SUCCESS);

            if (write(STDOUT_FILENO, buf, numRead) != numRead) {
                perror("FATAL partial/failed write (STDOUT_FILENO)");
                exit(EXIT_FAILURE);
            }

        }
    }

    return 0;
}
