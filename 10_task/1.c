#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    CHILD = 0,
    BUF_SIZE = 4096,
    WARNING_LENGTH = 7,
    ERROR_LENGTH = 5
} constants;

// find program.c:line:raw: error or warning
int64_t
need_to_increase(char word[], char file_name[])
{
    if (strncmp(word, file_name, strlen(file_name)) == 0) {
        int64_t line = 0;
	int64_t raw = 0;
	if (sscanf(word + strlen(file_name), ":%"PRId64":%"PRId64":", &line, &raw) == 2) {
	    return line;
	}
    }
    return -1;
}

int
main(int argc, char *argv[])
{
    char *file_name = argv[1];
    int pipe_fds[2];
    pid_t pid = 0;

    pipe(pipe_fds);

    pid = fork();

    // child try to compile
    // parent - to find count

    if (pid == CHILD) {
        dup2(pipe_fds[1], STDERR_FILENO);
	close(pipe_fds[1]);
	execlp("gcc", "gcc",  file_name, NULL);
    } else {
        char buffer[BUF_SIZE];
	waitpid(pid, 0, 0);
        uint64_t warnings = 0;
	uint64_t errors = 0;
	char *warning = "warning";
	char *error = "error";
	int64_t previous_warning_line = -1;
        int64_t previous_error_line = -1;
        int64_t line = 0;

        close(pipe_fds[1]);

        dup2(pipe_fds[0], STDIN_FILENO);
	close(pipe_fds[0]);

	while (scanf("%s", buffer) != EOF) {
	    if ((line = need_to_increase(buffer, file_name)) != -1) {
	        scanf("%s", buffer);
		if (line != previous_warning_line && strncmp(buffer, warning, WARNING_LENGTH) == 0) {
		    ++warnings;
		    previous_warning_line = line;
		}
		if (line != previous_error_line && strncmp(buffer, error, ERROR_LENGTH) == 0) {
		    ++errors;
		    previous_error_line = line;
		}
	    }
	}

	printf("%"PRIu64" %"PRIu64"\n", errors, warnings);
    }
    exit(0);
}
