#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <zconf.h>
#include <semaphore.h>

#include "common.h"

int shared_memory_fd;
sem_t* semaphore;

void handle_signal(int _) {
    printf("Closing barbershop\n");
    exit(0);
}

void invite_client() {
    pid_t client_pid = barbershop->queue[0];
    barbershop->selected_client = client_pid;
    printf("%lo barber: invited client %i\n", get_time(), client_pid);
}

void serve_client() {
    printf("%lo barber: started shaving client %i\n",
           get_time(),
           barbershop->selected_client);

    printf("%lo barber: finished shaving client %i\n",
           get_time(),
           barbershop->selected_client);

    barbershop->selected_client = 0;
}

void clean_up() {
    if (semaphore != 0) sem_unlink(PROJECT_PATH);
    if (shared_memory_fd != 0) shm_unlink(PROJECT_PATH);
}

void init(int argc, char** argv) {
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);
    atexit(clean_up);

    if (argc < 2)
        FAILURE_EXIT("Not enough arguments provided\n")

    int waiting_room_size = (int) strtol(argv[1], 0, 10);
    if (waiting_room_size > MAX_QUEUE_SIZE)
        FAILURE_EXIT("Provided room size was too big\n")

    shared_memory_fd = shm_open(
        PROJECT_PATH,
        O_RDWR | O_CREAT | O_EXCL,
        S_IRWXU | S_IRWXG
    );

    if (shared_memory_fd == -1)
        FAILURE_EXIT("Couldn't create shared memory\n")

    // Truncate file
    int error = ftruncate(shared_memory_fd, sizeof(*barbershop));

    if (error == -1)
        FAILURE_EXIT("Failed truncating file\n");

    // Access shared memory
    barbershop = mmap(
        NULL,                   // address
        sizeof(*barbershop),    // length
        PROT_READ | PROT_WRITE, // prot (memory segment security)
        MAP_SHARED,             // flags
        shared_memory_fd,       // file descriptor
        0                       // offset
    );

    if (barbershop == (void*) -1)
        FAILURE_EXIT("Couldn't access shared memory\n")

    // Open semaphore
    semaphore = sem_open(
        PROJECT_PATH,                // path
        O_WRONLY | O_CREAT | O_EXCL, // flags
        S_IRWXU | S_IRWXG,           // mode
        0                            // value
    );

    if (semaphore == (void*) -1)
        FAILURE_EXIT("Couldn't create semaphore\n")

    // Initialize the barbershop
    barbershop->barber_status = SLEEPING;
    barbershop->waiting_room_size = waiting_room_size;
    barbershop->client_count = 0;
    barbershop->selected_client = 0;

    // Initialize empty clients queue
    for (int i = 0; i < MAX_QUEUE_SIZE; ++i) barbershop->queue[i] = 0;
}


int main(int argc, char** argv) {
    init(argc, argv);

    release_semaphore(semaphore);

    while(1) {
        get_semaphore(semaphore);

        switch (barbershop->barber_status) {
            case IDLE:
                if (is_queue_empty()) {
                    printf("%lo barber: barber fell asleep\n", get_time());
                    barbershop->barber_status = SLEEPING;
                } else {
                    // Invite client from queue and then go to ready state
                    // (which will make the barber serve the customer)
                    invite_client();
                    barbershop->barber_status = READY;
                }
                break;
            case AWOKEN:
                printf("%lo barber: woke up\n", get_time());
                barbershop->barber_status = READY;
                break;
            case BUSY:
                serve_client();
                barbershop->barber_status = READY;
                break;
            default:
                break;
        }

        release_semaphore(semaphore);
    }
}