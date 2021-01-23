#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <inttypes.h>
#include <stdlib.h>

typedef enum {
    INPUT_SIZE = 4096,
    START_FORK = 1,
    CHILD = 0
} constants;


int
main()
{
    char input[INPUT_SIZE];
    pid_t pid = 0;
    u_int32_t fork_number = START_FORK;   //shows deep of recursion

    while (pid == 0 && scanf("%s", input) != EOF) {
	++fork_number;
	pid = fork();

        // if in parent, wait for child 
        // last child return 0, because he can't read anything
        // every child increase return status for 1. 
	// In parent we will have answer - 1, because we 
	// have already read one word. 

	if (pid != CHILD) {
	    int status;
            waitpid(pid, &status, 0);
            --fork_number;
            if (fork_number == START_FORK) {
                printf("%d\n", WEXITSTATUS(status) + 1);
	        exit(0);
            } else {
                exit(WEXITSTATUS(status) + 1);
    	    }
	}
    }

    // if we come here, no word is read 
    // if it is in first fork, than there is no words.
    // else it is just last child

    if (fork_number == START_FORK) {
        printf("0\n");
    }
    exit(0);

}
