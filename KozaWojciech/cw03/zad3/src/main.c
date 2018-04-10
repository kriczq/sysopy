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

long get_time(struct timeval *t) {
    return t->tv_sec * 1000000 + t->tv_usec;
}

void print_usage(struct rusage before, struct rusage after) {
    printf("user: %ldµs system: %ldµs\n\n", (get_time(&after.ru_utime) - get_time(&before.ru_utime)), (get_time(&after.ru_stime) - get_time(&before.ru_stime)));
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "too few arguments");
        exit(EXIT_FAILURE);
    }

    //set limits
    struct rlimit cpu_limit, mem_limit;

    cpu_limit.rlim_cur = cpu_limit.rlim_max = (rlim_t) atoi(argv[2]);
    mem_limit.rlim_cur = mem_limit.rlim_max = ((rlim_t) atoi(argv[3]) * 1024 * 1024);

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
        struct rusage before, after;

        printf("executing %s:\n", buffer);

        for (char *p = strtok(buffer, " \n\t"); p != NULL; p = strtok(NULL, " \n\t"))
            parameters[arg_index++] = p;

        parameters[arg_index] = NULL;

        getrusage(RUSAGE_CHILDREN, &before);

        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "fork failed");
			exit(EXIT_FAILURE);
        } else if (pid == 0) {
            if (setrlimit(RLIMIT_AS, &mem_limit) != 0 || setrlimit(RLIMIT_CPU, &cpu_limit) != 0) {
                fprintf(stderr, "cant set limits");
			    exit(EXIT_FAILURE);
            }

            execvp(parameters[0], parameters);
        } else {
            int status;
            waitpid(pid, &status, 0);

            getrusage(RUSAGE_CHILDREN, &after);
            print_usage(before, after);
        }
    }

    return 0;
}