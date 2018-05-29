#ifndef __COMMON_H__
#define __COMMON_H__

#include <errno.h>
#include <string.h>

// PRINTING UTILITIES //////////////////////////////////////////////////////////

#define RED_COLOR "\e[1;31m"
#define RESET_COLOR "\e[0m"

#define FAILURE_EXIT(format, ...) {                                            \
    printf(RED_COLOR format RESET_COLOR, ##__VA_ARGS__);                       \
    exit(-1);                                                                  }

// PROJECT CONSTS //////////////////////////////////////////////////////////////

#define PROJECT_PATH "/barbershop10"

// NOTE: actual size of the queue will be determined by the waiting_room_size
#define MAX_QUEUE_SIZE 64

// DATA ////////////////////////////////////////////////////////////////////////

enum Barber_status {
    SLEEPING,
    AWOKEN,
    READY,
    IDLE,
    BUSY
};

enum Client_status {
    NEWCOMER,
    INVITED,
    SHAVED
};

struct Barbershop {
    enum Barber_status barber_status;
    int client_count;
    int waiting_room_size;
    pid_t selected_client;
    pid_t queue[MAX_QUEUE_SIZE];
} *barbershop;

// COMMON HELPERS //////////////////////////////////////////////////////////////

long get_time() {
    struct timespec buffer;
    clock_gettime(CLOCK_MONOTONIC, &buffer);
    return buffer.tv_nsec / 1000;
}

// SEMAPHORE UTILITIES /////////////////////////////////////////////////////////

void get_semaphore(sem_t* semaphore) {
    int error = sem_wait(semaphore);
    if (error == -1) FAILURE_EXIT("Semaphore error: %s", strerror(errno))
}

void release_semaphore(sem_t* semaphore) {
    int error = sem_post(semaphore);
    if (error == -1) FAILURE_EXIT("Semaphore error: %s", strerror(errno))
}

// QUEUE UTILITIES /////////////////////////////////////////////////////////////

int is_queue_full() {
    if (barbershop->client_count < barbershop->waiting_room_size) return 0;
    return 1;
}

int is_queue_empty() {
    if (barbershop->client_count == 0) return 1;
    return 0;
}

void enter_queue(pid_t pid) {
    barbershop->queue[barbershop->client_count] = pid;
    barbershop->client_count += 1;
}

void pop_queue() {
    for (int i = 0; i < barbershop->client_count - 1; ++i) {
        barbershop->queue[i] = barbershop->queue[i + 1];
    }

    barbershop->queue[barbershop->client_count - 1] = 0;
    barbershop->client_count -= 1;
}

#endif