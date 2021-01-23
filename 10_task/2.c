#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
   CHILD = 0
} constants;

int
main(int argc, char *argv[])
{
    char *command_first = argv[1];
    char *command_second = argv[2];

    int pipe_fds[2];

    //create pipe
   
    pipe(pipe_fds);

    pid_t pid = CHILD;

    //make new process
    pid = fork();

    if (pid == CHILD) {
        //move stdout to write
	dup2(pipe_fds[1], 1);
	close(pipe_fds[1]);
	execlp(command_first, command_first, NULL);
    } else {
        waitpid(pid, 0, 0);

	//make other process for second command
	pid = fork();

	if (pid == CHILD) {
	    close(pipe_fds[1]);
	    //copy pipe to stdin
	    dup2(pipe_fds[0], 0); 
	    close(pipe_fds[0]);
            execlp(command_second, command_second, NULL);
	} else {
	    waitpid(pid, 0, 0);
	}
    }
    return 0;
}
