#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <zconf.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>

#include "common.h"

// GLOBALS /////////////////////////////////////////////////////////////////////

enum Client_status status;
int shared_memory_id;
int semaphore_id;

// UTILITIES ///////////////////////////////////////////////////////////////////

void init() {
    key_t project_key = ftok(PROJECT_PATH, PROJECT_ID);
    if (project_key == -1)
        FAILURE_EXIT("Couldn't obtain a project key\n")

    shared_memory_id = shmget(project_key, sizeof(struct Barbershop), 0);
    if (shared_memory_id == -1)
        FAILURE_EXIT("Couldn't create shared memory\n")

    barbershop = shmat(shared_memory_id, 0, 0);
    if (barbershop == (void*) -1)
        FAILURE_EXIT("Couldn't access shared memory\n")

    semaphore_id = semget(project_key, 0, 0);
    if (semaphore_id == -1)
        FAILURE_EXIT("Couldn't create semaphore\n")
}

void claim_chair() {
    pid_t pid = getpid();

    if (status == INVITED) {
        pop_queue();
    } else if (status == NEWCOMER) {
        while (1) {
            release_semaphore(semaphore_id);
            get_semaphore(semaphore_id);
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

        get_semaphore(semaphore_id);

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
            release_semaphore(semaphore_id);
            return;
        }

        release_semaphore(semaphore_id);

        while(status < INVITED) {
            get_semaphore(semaphore_id);
            if (barbershop->selected_client == pid) {
                status = INVITED;
                claim_chair();
                barbershop->barber_status = BUSY;
            }
            release_semaphore(semaphore_id);
        }

        while(status < SHAVED) {
            get_semaphore(semaphore_id);
            if (barbershop->selected_client != pid) {
                status = SHAVED;
                printf("%lo #%i: shaved\n", get_time(), pid);
                barbershop->barber_status = IDLE;
                cuts++;
            }
            release_semaphore(semaphore_id);
        }
    }
    printf("%lo #%i: left barbershop after %i cuts\n", get_time(), pid, S);
}

// MAIN ////////////////////////////////////////////////////////////////////////

// Takes two arguments:
// clients number - how many clients should be generated
// S              - number of cuts each client expects before dying or whatever
int main(int argc, char** argv) {
    if (argc < 3) FAILURE_EXIT("Not enough arguments provided\n")
    int clients_number = (int) strtol(argv[1], 0, 10);
    int S = (int) strtol(argv[2], 0, 10);
    init();

    for (int i = 0; i < clients_number; ++i) {
        if (!fork()) {
            run_client(S);
            exit(0);
        }
    }
    while (wait(0)) if (errno != ECHILD) break;
}