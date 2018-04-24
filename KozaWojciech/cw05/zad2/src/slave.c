#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>

#define BUFFER_SIZE 128

pid_t pid;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "too few arguments\n");
        exit(EXIT_FAILURE);
    }

    pid = getpid();
    srand(time(NULL) ^ getpid());
    int n = (int) strtol(argv[2], NULL, 10);

    int fifo = open(argv[1], O_WRONLY);
    if (fifo < 0) 
    {
        fprintf(stderr, "slave: error opening FIFO\n");
        exit(EXIT_FAILURE);
    }

    // Print PID to stdout
    printf("slave %i: %i\n", n, pid);

    char buffers[2][BUFFER_SIZE];
    for (int i = 0; i < n; i++) {
        // Generate date using popen
        FILE *date = popen("date", "r");
        fgets(buffers[0], BUFFER_SIZE, date);
        pclose(date);

        sprintf(buffers[1], "%i\t%i\t%s", i, pid, buffers[0]);
        printf("writing to fifo: %s", buffers[1]);
        write(fifo, buffers[1], strlen(buffers[1]));
        fclose(date);

        // Wait random amount of time
        sleep(rand() % 4 + 2);
    }

    close(fifo);
    return 0;
}