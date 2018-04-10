#include "../inc/lib.h"
#include "stdio.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void generate(char* file, int num, size_t size) {
    copy_sys("/dev/urandom", file, num, size);
}


/*void sort_sys(char *in_file, int num, size_t record_size) {
    int descriptor = open(in_file, O_RDWR);
    
    if (descriptor == -1) {
        fprintf(stderr, "error opening file: %s\n", in_file);
        exit(EXIT_FAILURE);
    }

    char *pom = malloc((size_t) record_size);
    char *j_buff = malloc((size_t) record_size);

    for (int ACTUAL_COMPARE = record_size; ACTUAL_COMPARE >= 0; ACTUAL_COMPARE--) {
        for (int i = 1; i < num; i++) {
            lseek(descriptor, record_size * i + i, SEEK_SET);
            read(descriptor, pom, (size_t) record_size);

            int j;
            for (j = i - 1; j >= 0; j--) {
                lseek(descriptor, record_size * j + j, SEEK_SET);
                read(descriptor, j_buff, (size_t) record_size);
                if (j_buff[ACTUAL_COMPARE] > pom[ACTUAL_COMPARE]) {
                    lseek(descriptor, record_size * j + j, SEEK_SET);
                    read(descriptor, j_buff, (size_t) record_size);

                    lseek(descriptor, record_size * (j + 1) + (j + 1), SEEK_SET);
                    write(descriptor, j_buff, (size_t) record_size);
                } else
                    break;
            }

            lseek(descriptor, record_size * (j + 1) + (j + 1), SEEK_SET);
            write(descriptor, pom, (size_t) record_size);
        }
    }
    close(descriptor);
}*/

void sort_sys(char *filename, int num, size_t record_size){
    int handle = open(filename, O_RDWR);

    char* buf1 = malloc(record_size * sizeof(char));
    char* buf2 = malloc(record_size * sizeof(char));

    if(handle){
        for(int i=1; i < num; i++){
            lseek(handle,i*record_size,SEEK_SET);
            read(handle,buf1,record_size);
            int j=0;
            while(1){
                lseek(handle,j*record_size,SEEK_SET);
                read(handle,buf2,record_size);
                if(j>=i || buf1[0]<buf2[0]){
                    break;
                } 
                j++;
            }

            lseek(handle,-record_size,SEEK_CUR);
            write(handle,buf1,record_size);
            for(int k = j+1; k < i+1; k++){
                lseek(handle,k*record_size,SEEK_SET);
                read(handle,buf1,record_size);
                lseek(handle,-record_size,SEEK_CUR);
                write(handle,buf2,record_size);
                strncpy(buf2,buf1,record_size);
            }

        }
    }

    close(handle);
}

void sort_lib(char *filename, int num, size_t record_size){
    FILE* handle = fopen(filename,"r+");

    char* buf1 = malloc(record_size * sizeof(char));
    char* buf2 = malloc(record_size * sizeof(char));

    if(handle){
        for(int i=1; i < num; i++){
            fseek(handle,i*record_size,0);
            fread(buf1,sizeof(char),record_size,handle);
            int j=0;
            while(1){
                fseek(handle,j*record_size,0);
                fread(buf2,sizeof(char),record_size,handle);
                if(j>=i || buf1[0]<buf2[0]){
                    break;
                } 
                j++;
            }

            fseek(handle,-record_size,1);
            fwrite(buf1,sizeof(char),record_size,handle);
            for(int k = j+1; k < i+1; k++){
                fseek(handle,k*record_size,0);
                fread(buf1,sizeof(char),record_size,handle);
                fseek(handle,-record_size,1);
                fwrite(buf2,sizeof(char),record_size,handle);
                strncpy(buf2,buf1,record_size);
            }

        }
    }

    fclose(handle);
}

void copy_sys(char* src_filename, char* dest_filename, int num, size_t record_size) {
    int src_descriptor = open(src_filename, O_RDWR);

    if (src_descriptor == -1) {
        fprintf(stderr, "error opening file: %s\n", src_filename);
        exit(EXIT_FAILURE);
    }

    int dest_descriptor = creat(dest_filename, S_IRWXG | S_IRWXO | S_IRWXU);
    
    if (dest_descriptor == -1) {
        fprintf(stderr, "error opening file: %s\n", dest_filename);
        exit(EXIT_FAILURE);
    }

    char *buffer = malloc(record_size * sizeof(char));

    for (int i = 0; i < num; ++i) {
        if (read(src_descriptor, buffer, record_size) != record_size) {
            fprintf(stderr, "read error\n");
            exit(EXIT_FAILURE);
        }
        if (write(dest_descriptor, buffer, record_size) != record_size) {
            fprintf(stderr, "write error\n");
            exit(EXIT_FAILURE);
        }
    }

    free(buffer);
    close(src_descriptor);
    close(dest_descriptor);
}

void copy_lib(char* src_filename, char* dest_filename, int num, size_t record_size) {
    FILE *src_file = fopen(src_filename, "r");

    if (src_file == NULL) {
        fprintf(stderr, "error opening file: %s\n", src_filename);
        exit(EXIT_FAILURE);
    }
    
    FILE *dest_file = fopen(dest_filename, "wb");
    
    if (dest_file == NULL) {
        fprintf(stderr, "error opening file: %s\n", dest_filename);
        exit(EXIT_FAILURE);
    }

    char *buffer = malloc(record_size * sizeof(char));

    for (int i = 0; i < num; ++i) {
        if (fread(buffer, sizeof(char), record_size, src_file) != record_size) {
            fprintf(stderr, "read error\n");
            exit(EXIT_FAILURE);
        }
        if (fwrite(buffer, sizeof(char), record_size, dest_file) != record_size) {
            fprintf(stderr, "write error\n");
            exit(EXIT_FAILURE);
        }
    }

    free(buffer);
    fclose(src_file);
    fclose(dest_file);
}