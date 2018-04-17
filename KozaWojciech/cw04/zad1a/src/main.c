#define __USE_XOPEN
#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <zconf.h>
#include <string.h>

int is_paused = 0;

void sigtstp_handler(int signum, siginfo_t *siginfo, void *context) {
    if (is_paused) {
        is_paused = 0;
    } else {
        is_paused = 1;
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
    }                                    
}

void stop_prog() {
    
}                                                                 

void sigint_handler(int _signum) {
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

    while (1) {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        
        printf("Current local time and date: %s", asctime(timeinfo));
        sleep(1);

        if (is_paused) {
            sigset_t mask, oldmask;
            sigemptyset(&mask);
            sigaddset(&mask, SIGTSTP);

            /* Wait for a signal to arrive. */
            sigprocmask(SIG_BLOCK, &mask, &oldmask);
            sigsuspend(&oldmask);
            sigprocmask(SIG_SETMASK, &oldmask, NULL);
        }
    }
  
}