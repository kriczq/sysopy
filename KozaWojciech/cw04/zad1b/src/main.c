#define __USE_XOPEN
#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/errno.h>
#include <string.h>

pid_t child_pid;

void start() {
    pid_t pid = fork();
    
    if (pid < 0) {
        fprintf(stderr, "Error: %s: %s\n", strerror(errno), "Fork failed");
        exit(errno);
    } else if (pid == 0) { //child
        execlp("/bin/sh","/bin/sh", "-c", "while true; do date; sleep 1; done", (char *) NULL);
    } else {
        child_pid = pid;
    }
}

void sigtstp_handler(int signum, siginfo_t *siginfo, void *context) {
    if (child_pid > 0) {
        kill(child_pid, SIGKILL);
        child_pid = -1;

        printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu \n");
    } else {
        start();
    }                                 
}                                                             

void sigint_handler(int signum) {
    printf("Odebrano sygnał SIGINT \n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    time_t rawtime;
    struct tm *timeinfo;

    struct sigaction sa;
    sa.sa_sigaction = sigtstp_handler;
    sa.sa_flags = SA_SIGINFO;

    signal(SIGINT, &sigint_handler);
    sigaction(SIGTSTP, &sa, NULL);

    start();
    while (1) pause();
}