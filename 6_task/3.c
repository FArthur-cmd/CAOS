#define _FILE_OFFSET_BITS 64 //for working with big files
//should be before includes
#define _GNU_SOURCE //from man pathconf for PATH_MAX

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

void work_with_link(char *f_name)
{
    char *new_name;
    new_name = realpath(f_name, NULL); //copy realpath to file in result
    if (new_name != NULL) { //if file exists print it
        printf("%s\n", new_name);
    }
}

void work_with_regular(char *f_name)
{
    char new_name[PATH_MAX];
    strcpy(new_name, "link_to_");
    strcat(new_name, basename(f_name)); // concatenate link_to_ with basename to make link
    symlink(f_name, new_name); //create symlink
}

void work_on_file(char *f_name)
{
    struct stat stat_t; //info about file
    if (lstat(f_name, &stat_t) != -1) { //if we can get information
        if (S_ISLNK(stat_t.st_mode)) { //check if it is link
	    work_with_link(f_name);
	}
	if (S_ISREG(stat_t.st_mode)) { //check if file is regular
	    work_with_regular(f_name);
	}
    }
}	

void correct_name(char *f_name)
{
    for (uint64_t i = 0; i < PATH_MAX; ++i) {  //find \n symbol
        if (f_name[i] == '\n') {
	    f_name[i] = '\0';
            break;
	}
    }
}

int
main()
{
    char f_name[PATH_MAX]; //PATH_MAX in limits.h shows max length of path

    while (fgets(f_name, sizeof(f_name), stdin)) {    //read

        correct_name(f_name);
	work_on_file(f_name);
    }
    return 0;
}
