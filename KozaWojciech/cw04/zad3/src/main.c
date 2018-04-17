#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define _XOPEN_SOURCE

#ifndef __linux__
#define SIGRTMIN 50
#define SIGRTMAX 64
#endif

int received = 0;

int L;
int Type;

pid_t child;

int responded = 0;
int sent_signals_counter = 0;
int received_signals_counter = 0;

pid_t make_child(void); // spawn a child
void define_signal_handling(); // define handlers
// custom handlers
void response_handler(int signal);
void sigint_handler(int signal);

void respond_handler(int signal) {
    printf("Child received %dnth signal %d\n",++received, signal);
    kill(getppid(), signal);
}

void exit_handler(int signal) {
    printf("Child received exit signal %d\n", signal);
    exit(EXIT_SUCCESS);
}

void define_signal_handling(void) {
    struct sigaction *sg = calloc(1, sizeof(struct sigaction));

    sg->sa_flags = 0;//| SA_NODEFER;
    sg->sa_handler = &response_handler;
    sigaction(SIGUSR1, sg, NULL);
    int csig = SIGRTMIN + 1;
    sigaction(csig, sg, NULL);

    sg->sa_handler = &sigint_handler;
    sigaction(SIGINT, sg, NULL);

    free(sg);
}

pid_t make_child(void) {
    pid_t pid = fork();

    if (pid == 0) {
        signal(SIGUSR1, &respond_handler);
        signal(SIGRTMIN + 1, &respond_handler);
        signal(SIGUSR2, &exit_handler);
        signal(SIGRTMIN + 2, &exit_handler);

        while (1);
    } else if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else {
        return pid;
    }
}

void response_handler(int signal) {
    responded = 1;
    printf("parent received %dth signal %d\n", ++received_signals_counter, signal);
}

void sigint_handler(int signal) {
    (void) signal;
    int csig = SIGRTMIN + 2;
    kill(child, (Type == 3) ? csig : SIGUSR2);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "too few args\n");
        exit(EXIT_FAILURE);
    }

    L = atoi(argv[1]);
    Type = atoi(argv[2]);

    define_signal_handling();

    int signal1, signal2;

    if (Type == 3) {
        signal1 = SIGRTMIN + 1;
        signal2 = SIGRTMIN + 2;
    } else {
        signal1 = SIGUSR1;
        signal2 = SIGUSR2;
    }

    // spawn child
    child = make_child();

    sleep(1);

    // signal sending loop
    for (int i = 0; i < L; ++i) {
        printf("sending %dth signal %d to child\n", ++sent_signals_counter, signal1);

        responded = 0;
        kill(child, signal1); // send a signal to a child

        if (Type == 2) {
            while (!responded); // wait for response from a child
        }
    }

    // send ending signal and wait for a child to end
    kill(child, signal2);
    waitpid(child, NULL, WNOHANG);
}