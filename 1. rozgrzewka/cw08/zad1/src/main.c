#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

struct Data {
    int n, m, max, c, threads_count;
    int **image_in,
        **image_out;
    double** k;
};

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a > b ? b : a;
}

int div_ceil(int a, int b) {
    return a % b == 0 ? a / b : a / b + 1;
}

void print_time(struct Data* data, struct timespec start, struct timespec end) {
    unsigned long long ms = (end.tv_sec - start.tv_sec) * 1000000;
    ms += (end.tv_nsec - start.tv_nsec) / 1000;

    printf("%d threads %d filter %llu μs\n", data->threads_count, data->c, ms);
}

void load_image(char* path, struct Data* data) {
    FILE* file = fopen(path, "r");
    fscanf(
        file,
        "P2 %d %d %d",
        &data->n,
        &data->m,
        &data->max
    );

    data->image_in = calloc(data->m, sizeof(int*));
    data->image_out = calloc(data->m, sizeof(int*));

    for (int i = 0; i < data->m; ++i) {
        data->image_in[i] = calloc(data->n, sizeof(int));
        data->image_out[i] = calloc(data->n, sizeof(int));
        for (int j = 0; j < data->n; ++j) {
            fscanf(file, "%d ", &data->image_in[i][j]);
        }
    }

    fclose(file);
}

void load_filter(char* path, struct Data* data) {
    FILE* file = fopen(path, "r");
    fscanf(file, "%d\n", &data->c);

    data->k = calloc(data->c, sizeof(double*));
    for (int i = 0; i < data->c; ++i) {
        data->k[i] = calloc(data->c, sizeof(double));
        for (int j = 0; j < data->c; ++j) {
            fscanf(file, "%lf ", &data->k[i][j]);
        }
    }

    fclose(file);
}

void save_file(char* path, struct Data* data) {
    FILE* file = fopen(path, "w");
    fprintf(
        file,
        "P2\n%d %d\n%d\n",
        data->n,
        data->m,
        data->max
    );

    for (int i = 0; i < data->m; ++i) {
        for (int j = 0; j < data->n; ++j) {
            fprintf(file, "%d ", data->image_out[i][j]);
        }
    }

    fprintf(file, "\n");
}

int process_pixel(struct Data* data, int x, int y) {
    double sum = 0;
    for (int i = 0; i < data->c; ++i) {
        for (int j = 0; j < data->c; ++j) {
            sum += data->image_in[
                max(0, min(data->n - 1, max(1, x - div_ceil(data->c, 2) + i)))
            ][
                max(0, min(data->n - 1, max(1, y - div_ceil(data->c, 2) + j)))
            ] * data->k[i][j];
        }
    }
    return (int) lround(sum);
}

void process_batch(void* args) {
    struct params {
        struct Data* data;
        int start;
        int size;
    } *params = (struct params*) args;

    for (int i = params->start; i < params->start + params->size; ++i) {
        for (int j = 0; j < params->data->m; ++j) {
            params->data->image_out[i][j] = process_pixel(params->data, i, j);
        }
    }
}

void process_image(struct Data* data) {
    pthread_t thread_ids[data->threads_count];
    pthread_attr_t* attr = calloc(1, sizeof(pthread_attr_t));

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_REALTIME, &start_time);

    int batch_size = data->n / data->threads_count;
    
    struct {
        struct Data* data; int start; int size;
    } params[data->threads_count];

    for (int i = 0; i < data->threads_count; ++i) {
        int size = i == data->threads_count - 1 ? data->n - (data->threads_count - 1) * batch_size : batch_size;

        params[i].data = data;
        params[i].start = i * batch_size;
        params[i].size = size;

        pthread_attr_init(attr);

        pthread_create(
            thread_ids + i,
            attr,
            (void* (*)(void*)) &process_batch,
            (void*) &params[i]
        );

        pthread_attr_destroy(attr);
    }

    for (int i = 0; i < data->threads_count; ++i) {
        pthread_join(thread_ids[i], 0);
    }

    clock_gettime(CLOCK_REALTIME, &end_time);
    print_time(data, start_time, end_time);

    free(attr);
}

int main(int argc, char** argv) {
    if (argc < 5)
    {
        printf("not enough arguments\n");
        exit(EXIT_FAILURE);
    }

    struct Data data;
    data.threads_count = atoi(argv[1]);

    load_image(argv[2], &data);
    load_filter(argv[3], &data);

    process_image(&data);

    save_file(argv[4], &data);
}