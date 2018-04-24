#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>

#define FAILURE_EXIT(format, ...) { printf(format, ##__VA_ARGS__); exit(-1); }
#define BUFFER_SIZE 128

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "too few arguments\n");
        exit(EXIT_FAILURE);
    } 

    if (mkfifo(argv[1], S_IRUSR | S_IWUSR) == -1) {
        FAILURE_EXIT("master: error creating FIFO\n")
    }

    //printf("master: sending signal to main (%i)\n", getppid());
    //kill(getppid(), SIGUSR1); // Signal parent to start slaves

    FILE *fifo = fopen(argv[1], "r");
    if (!fifo) FAILURE_EXIT("master: error opening FIFO\n")

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, fifo) != 0) {
        write(1, buffer, strlen(buffer));
    }

    printf("master: ended reading FIFO\n");
    fclose(fifo);
    if (remove(argv[1])) FAILURE_EXIT("master: error deleting FIFO\n")
    return EXIT_SUCCESS;
}