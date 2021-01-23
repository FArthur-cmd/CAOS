#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <inttypes.h>

int
main(int argc, char *argv[])
{
    //our number
    int64_t N = strtol(argv[1], NULL, 10);
    
    //shows number of fork. main fork number is 1
    int64_t forks_started = 0;                      

    pid_t pid = 0;
     
    // while we should create children
     //show next fork that it id next create child
     //if in child, make one more fork, if needed
   

    while (forks_started < N) {
        ++forks_started; 
	if (forks_started > 1) {
	    printf(" ");
	}	
	printf("%"PRId64"", forks_started);
        fflush(stdout);
	if (forks_started < N) {                       
	    pid = fork();
            //if parent, wait for child
	    if (pid > 0) {
	        waitpid(pid, 0, 0);
	        break;
	    }
	}
    }
    if (N == 1 || forks_started == 1) {
        printf("\n");
        fflush(stdout);
    }
    exit(0);
}
