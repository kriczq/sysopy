#define __USE_XOPEN
#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#include <time.h>
#include <dlfcn.h>
#include <memory.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ftw.h>
#include <sys/types.h>
#include <memory.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>


void strmode(mode, p)
	register mode_t mode;
	register char *p;
{
	 /* print type */
	switch (mode & S_IFMT) {
	case S_IFDIR:			/* directory */
		*p++ = 'd';
		break;
	case S_IFCHR:			/* character special */
		*p++ = 'c';
		break;
	case S_IFBLK:			/* block special */
		*p++ = 'b';
		break;
	case S_IFREG:			/* regular */
		*p++ = '-';
		break;
	case S_IFLNK:			/* symbolic link */
		*p++ = 'l';
		break;
	case S_IFSOCK:			/* socket */
		*p++ = 's';
		break;
#ifdef S_IFIFO
	case S_IFIFO:			/* fifo */
		*p++ = 'p';
		break;
#endif
#ifdef S_IFWHT
	case S_IFWHT:			/* whiteout */
		*p++ = 'w';
		break;
#endif
	default:			/* unknown */
		*p++ = '?';
		break;
	}
	/* usr */
	if (mode & S_IRUSR)
		*p++ = 'r';
	else
		*p++ = '-';
	if (mode & S_IWUSR)
		*p++ = 'w';
	else
		*p++ = '-';
	switch (mode & (S_IXUSR | S_ISUID)) {
	case 0:
		*p++ = '-';
		break;
	case S_IXUSR:
		*p++ = 'x';
		break;
	case S_ISUID:
		*p++ = 'S';
		break;
	case S_IXUSR | S_ISUID:
		*p++ = 's';
		break;
	}
	/* group */
	if (mode & S_IRGRP)
		*p++ = 'r';
	else
		*p++ = '-';
	if (mode & S_IWGRP)
		*p++ = 'w';
	else
		*p++ = '-';
	switch (mode & (S_IXGRP | S_ISGID)) {
	case 0:
		*p++ = '-';
		break;
	case S_IXGRP:
		*p++ = 'x';
		break;
	case S_ISGID:
		*p++ = 'S';
		break;
	case S_IXGRP | S_ISGID:
		*p++ = 's';
		break;
	}
	/* other */
	if (mode & S_IROTH)
		*p++ = 'r';
	else
		*p++ = '-';
	if (mode & S_IWOTH)
		*p++ = 'w';
	else
		*p++ = '-';
	switch (mode & (S_IXOTH | S_ISVTX)) {
	case 0:
		*p++ = '-';
		break;
	case S_IXOTH:
		*p++ = 'x';
		break;
	case S_ISVTX:
		*p++ = 'T';
		break;
	case S_IXOTH | S_ISVTX:
		*p++ = 't';
		break;
	}
	*p++ = ' ';		/* will be a '+' if ACL's implemented */
	*p = '\0';
}

char LOCAL_PATH[256];
struct tm TIME_ARGS;
int HOW;

void CHECK(bool is_ok, const char *message) {
    if (!is_ok) {
        fprintf(stderr, "Error: %s: %s\n", strerror(errno), message);
        exit(errno);
    }
}

void check_if_exists(const char *dirpath) {
    struct stat sb;

    if (!(stat(dirpath, &sb) == 0 && S_ISDIR(sb.st_mode))) {
        fprintf(stderr, "Can't find given directory: %s\n", dirpath);
        exit(1);
    }
}

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
    if (tm1.tm_year < tm2.tm_year) return -1;
    if (tm1.tm_year > tm2.tm_year) return 1;
    if (tm1.tm_mon < tm2.tm_mon) return -1;
    if (tm1.tm_mon > tm2.tm_mon) return 1;
    if (tm1.tm_mday < tm2.tm_mday) return -1;
    if (tm1.tm_mday > tm2.tm_mday) return 1;
    return 0;
}

void traverse(char *absolute_path) {
    DIR *dir = opendir(absolute_path);

    if (dir == NULL) {
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            pid_t pid;
            pid = fork();

            CHECK(pid >= 0, "Error creating process");

            if (pid == 0) {
                snprintf(LOCAL_PATH, sizeof(LOCAL_PATH), "%s/%s", absolute_path, entry->d_name);
                traverse(LOCAL_PATH);
                exit(0);
            } else {
                int status;
                wait(&status);
                if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                    perror("Something went wrong in child process.\n");
                }
            }
        } else if (entry->d_type == DT_REG) {
            struct stat *buf;
            buf = malloc(sizeof(struct stat));
            snprintf(LOCAL_PATH, sizeof(LOCAL_PATH), "%s/%s", absolute_path, entry->d_name);
            int file_stat = stat(LOCAL_PATH, buf);
            struct tm *time_file = localtime(&buf->st_mtime);
            if (file_stat == 0 && compare_tm(*time_file, TIME_ARGS) == HOW) {
                char bits_buff[20];
                char time_buff[20];
                mode_t bits = buf->st_mode;
                strmode(bits, bits_buff);
                convert_time(buf->st_mtime, time_buff);
                printf("%s %-10zd %-5s %-5s\n", bits_buff, buf->st_size, time_buff, LOCAL_PATH);
            }
            free(buf);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "too few arguments");
        exit(EXIT_FAILURE);
    }

    const char *dirpath = argv[1];
    const char *comparator = argv[2];
	
	char* ret = strptime(argv[3], "%Y-%m-%d", &TIME_ARGS);

	if (ret == NULL || *ret != '\0') {
		fprintf(stderr, "wrong date\n");
        exit(EXIT_FAILURE);
	}

	

    //TIME_ARGS = convert_to_tm(argv[3]);

    if (strcmp(comparator, "<") == 0)
        HOW = -1;
    else if (strcmp(comparator, "=") == 0)
        HOW = 0;
    else if (strcmp(comparator, ">") == 0)
        HOW = 1;
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

	traverse(absolute_path);

    return 0;
}