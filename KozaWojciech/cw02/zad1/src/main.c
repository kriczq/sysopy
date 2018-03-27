#include "../inc/lib.h"
#include "stdio.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <dlfcn.h>

static struct timeval real;
static struct rusage usage;

long get_time(struct timeval *t) {
    return t->tv_sec * 1000000 + t->tv_usec;
}

void before_test() {
    gettimeofday(&real, 0); 
    getrusage(RUSAGE_SELF, &usage);
}

void after_test() {
    long s_start = get_time(&usage.ru_stime);
    long u_start = get_time(&usage.ru_utime);
    long r_start = get_time(&real);

    before_test();

    long s_end = get_time(&usage.ru_stime);
    long u_end = get_time(&usage.ru_utime);
    long r_end = get_time(&real);  

    printf("real: %ldµs user: %ldµs system: %ldµs\n\n", (r_end - r_start), (u_end - u_start), (s_end - s_start));
}

int main(int argc, char* argv[]) {
    static struct option long_options[] = {
            {"generate",   no_argument, 0,  'g' },
            {"sort", no_argument, 0, 's'},
            {"copy", no_argument,       0,  'c' },
            {"lib", no_argument,       0,  'l' },
            {"src", required_argument,       0,  'f' },
            {"dest", required_argument,       0,  'd' },
            {"num", required_argument,       0,  'n' },
            {"record", required_argument,       0,  'r' },
            {0,           0,                 0,  0   }
    };

    int opt;
    int long_index = 0;

    char *src, *dest;
    char whattodo;
    int lib = 0, num = 0;
    size_t record_size;

    while ((opt = getopt_long(argc, argv, "gsclf:d:n:r:",
                              long_options, &long_index )) != -1) {
        
        switch (opt) {
            case 'g':   whattodo = opt;
                break;
            case 's':   whattodo = opt;
                break;
            case 'c':   whattodo = opt;
                break;
            case 'l':   lib = 1;
                break;
            case 'f':   src = optarg;
                break;
            case 'd':   dest = optarg;
                break;
            case 'n':   num = atoi(optarg);
                break;
            case 'r':   record_size = atoi(optarg);
                break;
            default: printf("Incorrect argument.\n");
                exit(EXIT_FAILURE);
        }

    }

    before_test();

    switch (whattodo) {
        case 'g':   
        printf("generating file %s, %d records of size %zu\n", src, num, record_size);
        generate(src, num, record_size);
            break;
        case 's':   
        printf("sorting file %s, %d records of size %zu using %s\n", src, num, record_size, (lib == 1) ? "lib" : "sys");
        (lib == 1) ? sort_lib(src, num, record_size) : sort_sys(src, num, record_size);
            break;
        case 'c':   
        printf("copying file %s to file %s, %d records of size %zu using %s\n", src, dest, num, record_size, (lib = 1) ? "lib" : "sys");
        (lib == 1) ? copy_lib(src, dest, num, record_size) : copy_sys(src, dest, num, record_size);
    }

    after_test();

    return 0;
}