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

static inline void
close_pipe(int created_pipe[])
{
    close(created_pipe[0]);
    close(created_pipe[1]);
}

static inline void
copy_fds(int from[], int to[])
{
    to[0] = from[0];
    to[1] = from[1];
}

static inline void
execute_command(char *cmd, int pipe_first[], int pipe_second[])
{
    pid_t pid = fork();

    if (pid == CHILD) {
	// send input from stdin to programm
	dup2(pipe_first[0], 0);
	close_pipe(pipe_first);

	//send output in next pipe
        dup2(pipe_second[1], 1);
	close_pipe(pipe_second);

	execlp(cmd, cmd, NULL);

    } else {
        close_pipe(pipe_first);
	
        //wait child
	waitpid(pid, 0, 0);
	//copy output to second input
	copy_fds(pipe_second, pipe_first);
    }
}

static inline void
execute_first_command(char *cmd, int pipe_first[], int pipe_second[])
{
    pid_t pid = fork();

    if (pid == CHILD) {
	close_pipe(pipe_first);

	//send output in next pipe
        dup2(pipe_second[1], 1);
	close_pipe(pipe_second);

	execlp(cmd, cmd, NULL);

    } else {
        close_pipe(pipe_first);
	
        //wait child
	waitpid(pid, 0, 0);
	//copy output to second input
	copy_fds(pipe_second, pipe_first);
    }
}

static inline void
execute_last_command(char *cmd, int pipe_first[], int pipe_second[])
{
    pid_t pid = fork();

    if (pid == CHILD) {
	// send input from stdin to programm
	dup2(pipe_first[0], 0);
	close_pipe(pipe_first);
	close_pipe(pipe_second);
	execlp(cmd, cmd, NULL);

    } else {
	//close pipes, because we don't need them
	close_pipe(pipe_first);
	close_pipe(pipe_second);
	//wait
	waitpid(pid, 0, 0);
    }
}

int
main(int argc, char *argv[])
{
    u_int32_t number_of_args = argc - 1;
    u_int32_t current_command = 1;
    int pipe_fds_first[2];
    int pipe_fds_second[2];
    

    // create pipe to execute first

    pipe(pipe_fds_first);
    pipe(pipe_fds_second);
    
    //first process
    execute_first_command(argv[current_command], pipe_fds_first, pipe_fds_second);
    ++current_command;
    pipe(pipe_fds_second);

    while (current_command < number_of_args) {
        execute_command(argv[current_command], pipe_fds_first, pipe_fds_second);
	//create new pipe because previous is closed
	pipe(pipe_fds_second);
	++current_command;
    }

    //last process
    execute_last_command(argv[current_command], pipe_fds_first, pipe_fds_second);
    exit(0);
} 
