#define __USE_XOPEN
#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <bsd/string.h>
#include <getopt.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <dlfcn.h>
#include <memory.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ftw.h>
#include <sys/types.h>



struct tm DATE_CMP;
int CMP;

char *convert_time(time_t time, char *buff) {
    struct tm *timeinfo;

    timeinfo = localtime(&time);
    strftime(buff, 20, "%F", timeinfo);
    return buff;
}

struct tm convert_to_tm(char *time_details) {
    struct tm tm;

    strptime(time_details, "%F", &tm);
    return tm;
}

int compare_tm(struct tm tm1, struct tm tm2) {
    if (tm1.tm_year < tm2.tm_year)
        return -1;
    if (tm1.tm_year > tm2.tm_year)
        return 1;

    if (tm1.tm_mon < tm2.tm_mon)
        return -1;
    if (tm1.tm_mon > tm2.tm_mon)
        return 1;

    if (tm1.tm_mday < tm2.tm_mday)
        return -1;
    if (tm1.tm_mday > tm2.tm_mday)
        return 1;
    else
        return 0;
}

int fn(const char *path, const struct stat *buf, int flag, struct FTW *ftw) {
    if (flag == FTW_F) {
        struct tm *time_file = localtime(&buf->st_mtime);

        if (compare_tm(*time_file, DATE_CMP) == CMP) {
            char bits_buff[20], time_buff[20];
            mode_t bits = buf->st_mode;

            strmode(bits, bits_buff);
            convert_time(buf->st_mtime, time_buff);
            printf("%s %-10zd %-5s %-5s\n", bits_buff, buf->st_size, time_buff, path);
        }
    }

    return 0;
}

void traverse(char *absolute_path) {
    DIR *dir = opendir(absolute_path);

    if (dir == NULL) {
        fprintf(stderr, "failed to open dir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        char local_path[PATH_MAX];

        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            
            snprintf(local_path, sizeof(local_path), "%s/%s", absolute_path, entry->d_name);
            traverse(local_path);
        } else if (entry->d_type == DT_REG) {
            struct stat *buf;
            buf = malloc(sizeof(struct stat));

            snprintf(local_path, sizeof(local_path), "%s/%s", absolute_path, entry->d_name);

            struct tm *time_file = localtime(&buf->st_mtime);

            if (stat(local_path, buf) == 0 && compare_tm(*time_file, DATE_CMP) == CMP) {
                char bits_buff[20], time_buff[20];
                mode_t bits = buf->st_mode;

                strmode(bits, bits_buff);
                convert_time(buf->st_mtime, time_buff);
                printf("%s %-10zd %-5s %-5s\n", bits_buff, buf->st_size, time_buff, local_path);
            }

            free(buf);
        }
    }

    closedir(dir);
}

void traverse_nftw(const char *absolute_path) {
    if (absolute_path == NULL || *absolute_path == '\0') {
        fprintf(stderr, "path error");
        exit(EXIT_FAILURE);
    }

    if (nftw(absolute_path, fn, 1024, FTW_PHYS) == -1){
        fprintf(stderr, "nftw error");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "too few arguments");
        exit(EXIT_FAILURE);
    }

    const char *dirpath = argv[1];
    const char *comparator = argv[2];
    DATE_CMP = convert_to_tm(argv[3]);

    if (strcmp(comparator, "<") == 0)
        CMP = -1;
    else if (strcmp(comparator, "=") == 0)
        CMP = 0;
    else if (strcmp(comparator, ">") == 0)
        CMP = 1;
    else {
        fprintf(stderr, "comparator can be < = > only");
        exit(EXIT_FAILURE);
    }

    struct stat sb;
    char absolute_path[PATH_MAX + 1];

    if (!(stat(dirpath, &sb) == 0 && S_ISDIR(sb.st_mode) && realpath(dirpath, absolute_path) != NULL)) {
        fprintf(stderr, "cant find directory: %s", dirpath);
        exit(EXIT_FAILURE);
    }

    if (argc == 5 && strcmp(argv[4], "--nftw") == 0)
        traverse_nftw(absolute_path);
    else
        traverse(absolute_path);

    return 0;
}