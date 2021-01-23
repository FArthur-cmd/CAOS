#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <stdio.h>


int
main (int argc, char *argv[])
{
    const char * directory_path = argv[1];
    int day = strtol(argv[4], NULL, 10);
    int month = strtol(argv[3], NULL, 10);
    int year =  strtol(argv[2], NULL, 10);
    int directory_file_discript = open(directory_path, O_RDONLY | O_DIRECTORY);
    DIR *directory = fdopendir(directory_file_discript);
    struct dirent *entry; //pointer to file
    struct stat info;
    struct tm start_to_find_from;
    time_t start_of_time;
    time_t time_of_file;

    memset(&start_to_find_from, 0, sizeof(start_to_find_from)); //clear fields
    
    // get information from man mktime
    start_to_find_from.tm_year = year - 1900;
    start_to_find_from.tm_mon = month - 1;
    start_to_find_from.tm_mday = day;
    start_to_find_from.tm_isdst = -1; //to compare with local time

    start_of_time = mktime(&start_to_find_from);//make time mark from input data

    while ((entry=readdir(directory))) {
        fstatat(directory_file_discript, entry->d_name, &info, 0);
        time_of_file = info.st_mtime;
	if (time_of_file >= start_of_time) {  //compare time
	    printf("%s\n", entry->d_name);
	}
    }

    closedir(directory);
    close(directory_file_discript);
    return(0);
}
