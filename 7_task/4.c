#define _FILE_OFFSET_BITS 64 //for working with big files
//should be before includes
#define _GNU_SOURCE //from man pathconf for PATH_MAX

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <stdio.h>
#include <inttypes.h>
#include <errno.h>

void
set_attributes(char attributes[], struct stat *info) {
    strcpy(attributes, "-rwxrwxrwx");
    if (S_ISDIR(info->st_mode)){ //type of file
        attributes[0] = 'd';
    } else if (S_ISLNK(info->st_mode)) {
        attributes[0] = 'l';
    } else if (S_ISFIFO(info->st_mode)) {
        attributes[0] = 'p';
    } else if (S_ISBLK(info->st_mode)) {
        attributes[0] = 'b';
    } else if (S_ISSOCK(info->st_mode)) {
        attributes[0] = 's';
    } else if (S_ISCHR(info->st_mode)) {
        attributes[0] = 'c';
    }
    for (uint32_t i = 9; i > 0; --i) { //rights
        if ((info->st_mode & (1 << (i - 1))) == 0) {
	    attributes[10 - i] = '-';
	}
    }
}

int32_t
print_for_file(char *path, struct stat *info)
{
    char attributes[13];
    size_t references_count = info->st_nlink;
    struct passwd *user = getpwuid(info->st_uid);  
    struct group *group = getgrgid(info->st_gid); 
    set_attributes(attributes, info);
    char name[NAME_MAX + 4 + PATH_MAX]; 
    strcpy(name, basename(path)); //name of file
    if (attributes[0] =='l') {	  //if link
        strcat(name," -> ");
     	char link_to_name[PATH_MAX];
	if (readlink(path, link_to_name, PATH_MAX) == -1) {
	    perror(NULL);
            return 1;
	}
        strcat(name, link_to_name);
    }
    printf("%s %lu %s %s %lu %s\n", attributes, references_count, user->pw_name, group->gr_name, info->st_size, name);
    return 0;
}


int32_t
print_for_dir(char *path, struct stat *info)
{
     struct dirent **entry;
     char new_path[PATH_MAX];
     int64_t number_of_files = scandir(path, &entry, NULL, alphasort); //read all in alphabetic order
     int64_t counter = 0;
     if (number_of_files == -1) {
         perror(NULL);
	 return 1;
     }

     while (counter < number_of_files) {
	 if (entry[counter]->d_name[0] != '.') {
	     memset(new_path, '\0', PATH_MAX*sizeof(char));
	     strcpy(new_path, path);
	     strcat(new_path, "/");
	     strcat(new_path, entry[counter]->d_name); //create path to file
             if (lstat(new_path, info) == -1) {
                 perror(NULL);
	         goto free_memory;
             }
             if (print_for_file(new_path, info) != 0) {
free_memory:
		 free(entry[counter]);
	         while (counter < number_of_files) {
	             free(entry[number_of_files]);
		     ++counter;
	         }
	         free(entry);
	         return 1;
	     }
	 }
         free(entry[counter]);
	 ++counter;
     }
     free(entry);
     return 0;

}


int32_t
print_information(char *path)
{
    struct stat info;
    if (lstat(path, &info) == -1) {
        perror(NULL);
	return 1;
    }
    if (S_ISDIR(info.st_mode)) { 
        return print_for_dir(path, &info);
    } else {
        return print_for_file(path, &info);
    }
}



int
main(int argc, char *argv[])
{
    char *path = "."; //if no arguments show this dir
    if (argc > 1) { 
        path = argv[1];
    }
    int32_t status = print_information(path);
    return status;
}
