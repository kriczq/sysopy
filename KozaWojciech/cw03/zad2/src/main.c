#define __USE_XOPEN
#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <memory.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>

const int MAX_NR_OF_ARGS = 64;
const int MAX_LINE_SIZE = 256;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "too few arguments");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(argv[1], "r");

	if (file == NULL)
	{
		fprintf(stderr, "cant open file");
        exit(EXIT_FAILURE);
	}

    char buffer[MAX_LINE_SIZE];
    char *parameters[MAX_NR_OF_ARGS];

    while (fgets(buffer, MAX_LINE_SIZE, file)) {
        int arg_index = 0;

        for (char *p = strtok(buffer, " \n\t"); p != NULL; p = strtok(NULL, " \n\t"))
            parameters[arg_index++] = p;

        parameters[arg_index] = NULL;

        pid_t pid = fork();

		if (pid < 0) {
            fprintf(stderr, "fork failed");
			exit(EXIT_FAILURE);
        } else if (pid == 0) {
            execvp(parameters[0], parameters);
        }
		
        int status;
        wait(&status);
    }

    return 0;
}