#define _FILE_OFFSET_BITS 64 //for working with big files
//should be before includes
#define _GNU_SOURCE //from man pathconf for PATH_MAX

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <stdio.h>
#include <inttypes.h>

uint64_t
count_size(char *path)
{
    int64_t dir_fd = open(path, O_RDONLY | O_DIRECTORY); //try to open as di
    if (dir_fd == -1) { // if can't open size == 0
        return 0;
    }
    DIR *directory = fdopendir(dir_fd); //from 7_0 task
    struct dirent *watch_now;
    struct stat info;
    uint64_t size = 0;
    char new_path[PATH_MAX]; //for next probable directory

    while ((watch_now = readdir(directory))) {
        fstatat(dir_fd, watch_now->d_name, &info, 0);
	
	if (S_ISREG(info.st_mode)) { // if regular add size
	    size += info.st_size;
	}

	if (S_ISDIR(info.st_mode)) { // if directory go in it and add size
	    if (!strcmp(watch_now->d_name, ".") || !strcmp(watch_now->d_name, "..")) {
	        continue; //compare with special symbols to avoid infinite cycle
	    } //and not to watch in previous directory
	    memset(new_path, 0, PATH_MAX*sizeof(char)); //clear path
	    strcat(new_path, path); //make path = prev_name/next_dir_name
	    strcat(new_path, "/");
	    strcat(new_path, watch_now->d_name);
	    size += count_size(new_path);
	}

    }

    closedir(directory);

    return size;
}

int
main(int argc, char *argv[])
{
    char* dir_path = argv[1];
    printf("%"PRIu64"\n", count_size(dir_path));
    return 0;
}
