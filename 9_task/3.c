#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

typedef enum {
    INPUT_LENGTH = 4096,
    PROGRAM_LENGTH = 50,
    CHILD = 0
} constants;


//make str right (make \0 at the end)

static inline void
set_endl(char* input)
{
    char *end_of_line = memchr(input, '\n', INPUT_LENGTH * sizeof(char));
    if (NULL != end_of_line) {
        *end_of_line = '\0';
    }
}


int
execute_for_child(char *input)
{
    char *c_file_name = "phy017_9_3.c";
    char *execute_file_name = "phy017_9_3";
    
    //clear if we already have this files

    remove(c_file_name);
    remove(execute_file_name);

    //create, write code and compile
    int fd = open(c_file_name, O_RDWR | O_CREAT, 0664);
    /*if (fd == -1) {
        perror(NULL);
	return 1;
    }*/

    //write program

    size_t size = strlen(input)*sizeof(char)+PROGRAM_LENGTH*sizeof(char);
   
    //make memory
    ftruncate(fd, size);

    char* contents = mmap(NULL,
                          size,
	                  PROT_READ | PROT_WRITE,
	                  MAP_SHARED,
	                  fd, 
	                  0);

    sprintf(contents, "#include<stdio.h>\nint main(){\nprintf(\"%%d\\n\",(%s));\n}", input);
    

    close(fd);
    munmap(contents, size);
    // make executable

    execlp("gcc", "gcc", c_file_name, "-o", execute_file_name, NULL);

    /*
    if (-1 == execlp("gcc", "gcc", c_file_name, "-o", execute_file_name, NULL)) {
        perror(NULL);
	return -1;
    }
*/
    return 0;
}

int
execute_for_parent()
{
    char *activate_file = "./phy017_9_3";
   
    execl(activate_file, activate_file, NULL);
    /*
    if (-1 == execl(activate_file, activate_file, NULL)) {
        perror(NULL);
	return 1;
    }
    */
    return 0;
}

void
clear_files()
{
    char *c_file_name ="phy017_9_3.c";
    char *execute_file_name = "phy017_9_3";

    //delete all files

    remove(c_file_name);
    remove(execute_file_name);
}

int
main()
{
    char input[INPUT_LENGTH];
    pid_t pid = 0;
    int status = 0;

    fgets(input, INPUT_LENGTH * sizeof(char), stdin);

    /*
    if (NULL == fgets(input, INPUT_LENGTH * sizeof(char), stdin)) {      //read str
        perror(NULL);
        exit(1);
    }
    */

    set_endl(input);
    
    if (strlen(input) == 0) {
       return 0;
    }

    pid = fork();

    if (pid == CHILD) {
        status = execute_for_child(input);
    } else {
	int child_status;
	waitpid(pid, &child_status, 0);
	/*if (WEXITSTATUS(child_status) == 1){
	    exit(1);
	}*/
	pid = fork();
        status = 0;
        if (pid != CHILD) {
	    waitpid(pid, &child_status, 0);
            clear_files();
        } else {
     	   status = execute_for_parent();
	}
    }
   // exit(status);
   return status;
}

