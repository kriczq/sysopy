#include "../inc/lib.h"
#include "stdio.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#ifndef LIB_STATIC
array* create(int size_max, int block_size) {
    if (size_max <= 0 || block_size <= 0)
        return NULL;

    array* new_array = malloc(sizeof(array));

    new_array->block = malloc(size_max * sizeof(char *));
    new_array->size = 0;
    new_array->size_max = size_max;
    new_array->block_size = block_size;

    return new_array;
}
#endif

int delete_block(array* array, char * to_delete) {
    if (to_delete == NULL)
        return 0;
    
    for (int i = 0; i < array->size_max; ++i) {
        if (array->block[i] == to_delete) { //we can also use strcmp == 0 if we want to compare meaning not address
            
            #ifdef LIB_STATIC
                array->block[i][0] = '\0';
            #else
                free(array->block[i]);
                array->block[i] = NULL;
            #endif

            array->size--;

            return 1;
        }
    }
    
    return 0;
}

void delete(array* array) {
    for (int i = 0; i < array->size_max; ++i) {
        if (array->block[i] != NULL)
            delete_block(array, array->block[i]);
        
        #ifndef LIB_STATIC
        free(array);
        #endif
    }
}

char* add_block(array* array, char* to_add) {
    if (array->size >= array->size_max || strlen(to_add) == 0)
        return NULL;
    
    for (int i = 0; i < array->size_max; ++i) {
        if (array->block[i] == NULL || array->block[i][0] == '\0') {
            #ifndef LIB_STATIC
            array->block[i] = malloc((strlen(to_add) + 1) * sizeof(char));
            #endif

            strcpy(array->block[i], to_add);
            array->size++;

            return array->block[i];
        }
    }

    return NULL;
}

char* find(array* array, char * to_find) {
    int diff = INT_MAX;
    char *block = NULL;

    int to_find_sum = 0;

    for (int k = 0; to_find[k] != '\0'; ++k)
            to_find_sum += to_find[k];

    for (int i = 0; i < array->size_max; ++i) {
        int sum = 0;

        for (int k = 0; array->block[i][k] != '\0'; ++k)
            sum += array->block[i][k];
        
        if (abs(sum - to_find_sum) < diff) {
            diff = abs(sum - to_find_sum);
            block = array->block[i];
        }
    }

    return block;
}

void print(array* array) {
    for (int i = 0; i < array->size_max; ++i) {
        if (array->block[i] != NULL) printf("%s ,%lu\n", array->block[i], strlen(array->block[i]));
    }
}