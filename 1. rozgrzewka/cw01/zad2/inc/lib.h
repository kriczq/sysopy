#ifdef LIB_STATIC
    #define STATIC_ELEMENTS_MAX 1024
    #define STATIC_BLOCK_SIZE 256
#endif

typedef struct {
    #ifdef LIB_STATIC
        char block[STATIC_ELEMENTS_MAX][STATIC_BLOCK_SIZE];
    #else
        char** block;
    #endif
    
    int size;
    int size_max;
    int block_size;
} array;

#ifndef LIB_STATIC
    array * create(int, int);
#endif
char * add_block(array *, char *);
void delete(array *);
int delete_block(array *, char *);
void print(array *);
char * find(array *, char *);