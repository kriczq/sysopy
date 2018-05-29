#define _GNU_SOURCE

#include <stdbool.h>
#include <stdlib.h>

#ifndef SYSOPY4_COMMON_UTILS_H
#define SYSOPY4_COMMON_UTILS_H

#define CHECK_RETURN(EXPR) if(EXPR) { return -1; }
#define MAX_NR_CLIENTS 1024
#define SHM_CHAR 'b'
#define SEM_CHAR 's'
#define DATE_SIZE 128

#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAG     "\x1B[35m"
#define CYN     "\x1B[36m"
#define RESET   "\x1B[0m"

//BARBER
enum barber_status {
    SLEEPING,
    AWOKEN,
    BUSY,
    FREE
};

enum client_status {
    NONE,
    WAITING,
    INVITED,
    SITTING
};

typedef struct Barbershop {
    enum barber_status barber_status;
    int current_client;
    enum client_status current_client_status;
    pid_t queue[MAX_NR_CLIENTS];
    int clients_waiting;
    int queue_head;
    int queue_tail;
    int nr_seats;
} Barbershop;

void check_exit(bool correct, const char *message);
int to_int(char *string);
const char *get_homedir();
void sem_give(int sem_id);
void sem_take(int sem_id);
char *gettime(char buffer[DATE_SIZE]);

#endif