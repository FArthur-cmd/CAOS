#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

typedef enum {
    CHILD = 0,
    BUFFER_SIZE = 4096
} constants;

int
main(int argc, char *argv[])
{
    char *command = argv[1];
    char *in_file = argv[2];
    int in_fd = open(in_file, O_RDONLY);
    int pipe_fds[2];
    pid_t pid = 0;

    //move in to stdin
    dup2(in_fd, 0); 
    close(in_fd);

    //create pipe
    pipe(pipe_fds);

    //exec command in child
    pid = fork();

    if (pid == CHILD) {
        dup2(pipe_fds[1], 1);
	close(pipe_fds[1]);
	execlp(command, command, NULL);
    } else {
	waitpid(pid, 0, 0);
	    
	//close output to stop writing
        close(pipe_fds[1]);
	uint64_t count = 0;
	ssize_t current_read = 0;
	char buffer[BUFFER_SIZE];
	while ((current_read=read(pipe_fds[0], buffer, sizeof(buffer))) > 0) {
	    count += current_read;
	}
        printf("%"PRIu64"\n", count);
    }
    exit(0);
}
