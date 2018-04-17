#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_DELAY 10

int N;
int K;

sig_atomic_t request_counter = 0; // number of submitted requests
sig_atomic_t alive_children_counter = 0; // number of living children

pid_t *children = NULL; 
int *permission_granted = NULL; 
int delay;

int get_child_id(pid_t pid);
pid_t make_child(void); 
void define_signal_handling();
void send_allow(pid_t child); // send info to a child
void allow_all(void); // send info to all children with permission
void request_handler(int signal, siginfo_t *info, void *ucontext);
void sigchld_handler(int signal, siginfo_t *info, void *ucontext);
void rt_handler(int signal, siginfo_t *info, void *ucontext);
void sigint_handler(void);

void set_rand_delay() {
    delay = rand() % (MAX_DELAY + 1);
}

int rand_rt_int() {
    return SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN + 1));
}

void handler(int signal) {
    (void) signal;
    kill(getppid(), rand_rt_int());

    exit(delay);
}

pid_t make_child(void) {
    pid_t pid = fork();

    if (pid == 0) {
        set_rand_delay();

        signal(SIGALRM, &handler);

        sleep(delay);
        kill(getppid(), SIGUSR1);

        pause();
    } else if (pid == -1) {
        perror("fork error");
        exit(EXIT_FAILURE);
    } else {
        ++alive_children_counter;
        return pid;
    }
}

void define_signal_handling() {
    sigset_t to_mask;
    sigfillset(&to_mask);

    struct sigaction *sg = calloc(1, sizeof(struct sigaction));
    sg->sa_flags = SA_SIGINFO;
    sg->sa_sigaction = &request_handler;
    sg->sa_mask = to_mask;
    sigaction(SIGUSR1, sg, NULL); // handle signal from child

    sg->sa_sigaction = &sigchld_handler;
    sigaction(SIGCHLD, sg, NULL); // handle child termination

    sg->sa_sigaction = (void (*)(int, siginfo_t *, void *)) &sigint_handler;
    sigaction(SIGINT, sg, NULL);

    sg->sa_sigaction = &rt_handler;
    for (int sig = SIGRTMIN; sig <= SIGRTMAX; ++sig) {
        sigaction(sig, sg, NULL);
    }
    free(sg);
}

void allow_all(void) {
    for (int i = 0; i < N; ++i) {
        if (permission_granted[i]) {
            send_allow(children[i]);
        }
    }
}

void send_allow(pid_t child) {
    kill(child, SIGALRM);
}

void request_handler(int signal, siginfo_t *info, void *ucontext) {
    assert(signal == SIGUSR1);
    (void) ucontext;

    pid_t caller = info->si_pid;
    int childnum = get_child_id(caller);

    if (!permission_granted[childnum]) {
        ++request_counter;
        permission_granted[childnum] = 1;
    }

    if (request_counter == K) {
        allow_all();
    } else if (request_counter > K) {
        send_allow(caller);
    }
}


void sigchld_handler(int signal, siginfo_t *info, void *ucontext) {
    assert(signal == SIGCHLD);
    (void) ucontext;

    int left = --alive_children_counter;
    int status = info->si_status;
    pid_t child = info->si_pid;
    int i = get_child_id(child);

    children[i] = 0;

    assert(status <= 10);

    waitpid(info->si_pid, NULL, 0);
}

void rt_handler(int signal, siginfo_t *info, void *ucontext) {
    (void) ucontext;
    assert(signal >= SIGRTMIN && signal <= SIGRTMAX);
}

void sigint_handler(void) {
    for (int i = 0; i < N; ++i) {
        if (children[i] != 0) {
            kill(children[i], SIGKILL);
        }
    }
    exit(EXIT_SUCCESS);
}

int get_child_id(pid_t pid) {
    int id = 0;
    while (id < N && children[id] != pid) ++id;
    if (id < N) {
        return id;
    } else {
        return -1;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "too few parameters\n");
        exit(EXIT_FAILURE);
    }

    N = strtoul(argv[1], NULL, 10);
    K = strtoul(argv[2], NULL, 10);

    if (N < 1 || N < K) {
        fprintf(stderr, "incorrect N\n");
        exit(EXIT_FAILURE);
    }

    children = calloc(N, sizeof(pid_t));
    permission_granted = calloc(N, sizeof(int));

    define_signal_handling();

    for (int i = 0; i < N; ++i) {
        children[i] = make_child();
    }

    while (alive_children_counter > 0);

    free(children);
    free(permission_granted);

    return 0;
}