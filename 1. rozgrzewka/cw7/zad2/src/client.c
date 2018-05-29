#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <zconf.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>
#include <semaphore.h>

#include "common.h"

enum Client_status status;
int shared_memory_fd;
sem_t* semaphore;

void init() {
    shared_memory_fd = shm_open(PROJECT_PATH, O_RDWR, S_IRWXU | S_IRWXG);

    if (shared_memory_fd == -1)
        FAILURE_EXIT("Couldn't create shared memory\n")

    // Truncate file
    int error = ftruncate(shared_memory_fd, sizeof(*barbershop));

    if (error == -1)
        FAILURE_EXIT("Failed truncating file\n");

    barbershop = mmap(NULL,
                      sizeof(*barbershop),
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      shared_memory_fd,
                      0);

    if (barbershop == (void*) -1)
        FAILURE_EXIT("Couldn't access shared memory\n")

    semaphore = sem_open(PROJECT_PATH, O_WRONLY, S_IRWXU | S_IRWXG, 0);

    if (semaphore == (void*) -1)
        FAILURE_EXIT("Couldn't create semaphore\n")
}

void claim_chair() {
    pid_t pid = getpid();

    if (status == INVITED) {
        pop_queue();
    } else if (status == NEWCOMER) {
        while (1) {
            release_semaphore(semaphore);
            get_semaphore(semaphore);
            if (barbershop->barber_status == READY) break;
        }
        status = INVITED;
    }
    barbershop->selected_client = pid;
    printf("%lo: #%i: took place in the chair\n", get_time(), pid);
}

void run_client(int S) {
    pid_t pid = getpid();
    int cuts = 0;

    while (cuts < S) {
        // Handle client
        status = NEWCOMER;

        get_semaphore(semaphore);

        if (barbershop->barber_status == SLEEPING) {
            printf("%lo #%i: woke up the barber\n", get_time(), pid);
            barbershop->barber_status = AWOKEN;
            claim_chair();
            barbershop->barber_status = BUSY;
        } else if (!is_queue_full()) {
            enter_queue(pid);
            printf("%lo #%i: entering the queue\n", get_time(), pid);
        } else {
            printf("%lo #%i: could not find place in the queue\n", get_time(), pid);
            release_semaphore(semaphore);
            return;
        }

        release_semaphore(semaphore);

        while(status < INVITED) {
            get_semaphore(semaphore);
            if (barbershop->selected_client == pid) {
                status = INVITED;
                claim_chair();
                barbershop->barber_status = BUSY;
            }
            release_semaphore(semaphore);
        }

        while(status < SHAVED) {
            get_semaphore(semaphore);
            if (barbershop->selected_client != pid) {
                status = SHAVED;
                printf("%lo #%i: shaved\n", get_time(), pid);
                barbershop->barber_status = IDLE;
                cuts++;
            }
            release_semaphore(semaphore);
        }
    }
    printf("%lo #%i: left barbershop after %i cuts\n", get_time(), pid, S);
}

int main(int argc, char** argv) {
    if (argc < 3) 
        FAILURE_EXIT("Not enough arguments provided\n")

    int clients_number = (int) strtol(argv[1], 0, 10);
    int S = (int) strtol(argv[2], 0, 10);
    init();

    for (int i = 0; i < clients_number; i++) {
        if (!fork()) {
            run_client(S);
            exit(0);
        }
    }
    while (wait(0)) 
        if (errno != ECHILD)
            break;
}