#include "../inc/lib.h"
#include "stdio.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

static struct timeval real;
static struct rusage usage;
char buf[256];

#ifdef DYNAMIC
#include <dlfcn.h>

void* handle;

void load_lib() {
    handle = dlopen("../build/lib/lib_shared.so", RTLD_LAZY);

    if (!handle) {
        printf("failed to load library: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
}

void close_lib() {
    if (dlclose(handle) != 0) {
        printf("Problem closing library: %s", dlerror());
    }
}

#endif

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


//* create_table rozmiar rozmiar_bloku - stworzenie tablicy o rozmiarze "rozmiar" i blokach o rozmiarach "rozmiar bloku" 
void test_create_array(FILE * stream, int num, int block_size) {
    #ifdef DYNAMIC
        array* (*create)(int, int) = dlsym(handle, "create");
        char* (*add_block)(array *, char *) = dlsym(handle, "add_block");
    #endif

    printf("Blocks creation:\nNumber of elements: %d\nBlock size: %d\n",num,0);
    before_test();
    
    array *test = create(num, block_size);

    for (int i = 0; i < num; ++i) {
        fgets(buf, sizeof(buf), stream);

        add_block(test, buf);
    }

    after_test();
}

void test_del(FILE * stream, array * test, int num) {
    #ifdef DYNAMIC
        int (*delete_block)(array*, char *) = dlsym(handle, "delete_block");
    #endif

    printf("Deleting in sequence:\n");
    before_test();

    int deleted = 0;
    for (int i = 0; i < test->size_max && deleted < num; ++i) {
        if (delete_block(test, test->block[i]) == 1) {
            //printf("Deleted block no %d block address %p\n", no, to_delete);
            deleted++;
        }
    }

    printf("delted %d blocks of %d requested\n", deleted, num);
    after_test();
}

void test_add(FILE * stream, array * test, int num) {
    #ifdef DYNAMIC
        char * (*add_block)(array *, char *) = dlsym(handle, "add_block");
    #endif

    printf("adding in sequence:\n");
    before_test();

    for (int i = 0; i < num; ++i) {
        fgets(buf, sizeof(buf), stream);
        char * added = add_block(test, buf);
        //printf("Added block no %d on address %p\n", i, added);
    }

    after_test();
}

void test_del_add(FILE * stream, array * test, int num) {
    #ifdef DYNAMIC
        char * (*add_block)(array *, char *) = dlsym(handle, "add_block");
        int (*delete_block)(array *, char *) = dlsym(handle, "delete_block");
    #endif
    printf("Deleting and adding alternaly:\n");
    before_test();

    for (int i = 0; i < num;)
        if (delete_block(test, test->block[(rand() % test->size)]) == 1) {
            fgets(buf, sizeof(buf), stream);
            add_block(test, buf);
            ++i;
        }
    
    after_test();
}

void test_find(array * test, char * to_find) {
    #ifdef DYNAMIC
        char * (*find)(array *, char *) = dlsym(handle, "find");
    #endif

    printf("Finding element most similar to %s: ", to_find);
    before_test();

    char* result = find(test, to_find);

    printf("%s\n", result);
    after_test();
}

int main(int argc, char* argv[]) {
    #ifdef DYNAMIC
        printf("dynamiczna blibteka\n");
        load_lib();
    #endif

    #ifdef LIB_STATIC
        printf("statyczna lakoacja pamieci\n");
        array arrayx;
        array* test = &arrayx;
    #else
        if (argc < 3)
            return EXIT_FAILURE;
        
        #ifdef DYNAMIC
        array* (*create)(int, int) = dlsym(handle, "create");
        #endif
        array * test = create(atoi(argv[1]), atoi(argv[2]));
    #endif

    static struct option long_options[] = {
            {"static",   required_argument, 0,  's' },
            {"create", required_argument, 0, 'c'},
            {"add", required_argument,       0,  'a' },
            {"remove", required_argument,       0,  'r' },
            {"del_remove_alt", required_argument,       0,  'l' },
            {"find", required_argument,       0,  'f' },
            {0,           0,                 0,  0   }
    };

    int opt = 0;
    int long_index = 0;
    int static_allocation = 0;

    while ((opt = getopt_long(argc, argv, "sc:a:r:l:f:", //s:
                              long_options, &long_index )) != -1 && opt != 's');
    
    if (opt == 's') static_allocation = 1;
    optind = 0;

    static const char filename[] = "../data.txt";
    FILE * stream = fopen(filename, "r");

    while ((opt = getopt_long(argc, argv, "sc:a:r:l:f:",
                              long_options, &long_index )) != -1) {
        switch (opt) {
            case 's': break;
            case 'c':  test_create_array(stream, atoi(optarg), atoi(argv[optind]));
                break;
            case 'a':   test_add(stream, test, atoi(optarg));
                break;
            case 'r':   test_del(stream, test, atoi(optarg));
                break;
            case 'l':   test_del_add(stream, test, atoi(optarg));
                break;
            case 'f':   test_find(test, optarg);
                break;
            default: printf("Incorrect argument.\n");
                exit(EXIT_FAILURE);
        }

    }
    
    #ifdef DYNAMIC
        close_lib();
    #endif

    return 0;
}