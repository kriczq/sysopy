#define __USE_XOPEN
#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ftw.h>
#include <math.h>
#include <dirent.h>
#include <time.h>

#define NOOPENFD 1024

struct parsedArgs {
    char* path;
    char direction;
    char* date;
};

struct parsedArgs globalVar;
#define SGVBB globalVar

struct parsedArgs parser (int argc, char* argv[]) {
    if(argc < 3)
        exit(EXIT_FAILURE);
    struct parsedArgs args;

    args.path = argv[1];
    args.direction = *argv[2];
    args.date = argv[3];

    return args;
}

char* get_permissions(const struct stat* filestat) {

    char* permissions = (char*) malloc(sizeof(char) * 11);
    permissions[0] = S_ISDIR(filestat -> st_mode) ? 'd' : '-';
    permissions[1] = (filestat -> st_mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (filestat -> st_mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (filestat -> st_mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (filestat -> st_mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (filestat -> st_mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (filestat -> st_mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (filestat -> st_mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (filestat -> st_mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (filestat -> st_mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';
    return permissions;
}


void print_file_info(const struct stat* statBuffer,const char* absPath) {

    char* permissions =  (char*) get_permissions(statBuffer);

    printf("%-80s%-12lld%-8s      %-16s",
           absPath,
           (long long int) statBuffer->st_size,
           permissions,
           ctime(&(statBuffer->st_mtime))
    );
    free(permissions);
}


int date_compare (char* date, char direction, time_t fileTime) {

    struct tm* parsedDate = (struct tm*) malloc(sizeof(struct tm));

    char* ret = strptime(date, "%Y-%m-%d", parsedDate);
    if(ret == NULL || *ret != '\0') {
    	printf("wrong date\n");
        exit(EXIT_FAILURE);

    }

    time_t parsedTime = mktime(parsedDate);

    if(direction == '='){
        return fabs(difftime(parsedTime, fileTime)) < 0.001 ? 1 : 0;
    } else if(direction == '<')
    {
        return difftime(parsedTime, fileTime) > 0 ? 1 : 0;
    } else if(direction == '>')
    {
        return difftime(parsedTime, fileTime) < 0 ? 1 : 0;
    } else
        exit(EXIT_FAILURE);
}



void process_file_structure( char* path, char direction,char* date) {

           DIR* handle = opendir(path);
           struct dirent* currentDir = readdir(handle);

           while(currentDir != NULL) {

               if (strcmp(currentDir -> d_name, ".") == 0 || strcmp(currentDir -> d_name, "..") == 0) {
                   currentDir = readdir(handle);
               } else 

               {

               char* currentDirPath = (char*) calloc(PATH_MAX, sizeof(char));
               strcat(strcat(strcat(currentDirPath, path),"/"), currentDir->d_name);

               if(currentDir -> d_type == DT_DIR) {
                   process_file_structure(currentDirPath, direction, date);
               }

               
               else if(currentDir -> d_type == DT_REG) {


                    struct stat* statBuffer = (struct stat*) malloc(sizeof(struct stat));


                    if(stat(currentDirPath, statBuffer) != -1)
                    if(date_compare(date, direction, statBuffer -> st_mtime)) {
                       print_file_info(statBuffer, currentDirPath);

                   }
               }

               currentDir = readdir(handle);

               }
       }
 }

 int fn (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {

                if (typeflag == FTW_F && date_compare(SGVBB.date, SGVBB.direction, sb -> st_mtime))
                       print_file_info(sb, fpath);
                return 0;
            }


int main( int argc, char* argv[] ) {

    struct parsedArgs args = parser(argc, argv);

    DIR *check = opendir(argv[1]); //check if directiory is good
    if (check == 0) {
        printf("Cannot open a directory.\n");
        return EXIT_FAILURE;
    }
    closedir(check);


    if(!(args.direction == '=' || args.direction == '>' || args.direction == '<')) {
        printf("Wrong second argument \n");
        return EXIT_FAILURE;
    }
    #ifdef NFTW
        SGVBB = parser(argc, argv);
        nftw(SGVBB.path, fn, NOOPENFD, FTW_PHYS);
    #else
        process_file_structure(args.path, args.direction, args.date);
    #endif

}