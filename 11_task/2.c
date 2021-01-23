#include <sys/signalfd.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//files array starts from 1 element in argv
//need to ignore zero element in argv 
//and need to ignore SIGRTMIN

#define IGNORE_PROG_NAME(x) (x-1)
#define IGNORE_FIRST_NAME(x) (x+1)

/*
 * Max length of input string
 * TRUE for endless while
 */
typedef enum {
    MAX_LENGTH = 4096,
    RUN = 1
} constants;

/*
 * read and write function
 * read from file and print
 */
static inline void
RDW(FILE *from) 
{
    char line[MAX_LENGTH];
    memset(line, '\0', sizeof(line));
    fgets(line, sizeof(line), from);
    fputs(line, stdout);
    fflush(stdout);
}

// open all files from argv
static inline void
open_all(FILE **files, int count, char *names[])
{
   for (int i = 0; i < count; ++i) {
       files[i] = fopen(names[i], "r");
   }
}

// set masks to get only [SIGRTMIN; SIGRTMAX]
static inline void
set_masks(sigset_t *in, sigset_t *ignore, int possible_count)
{
    //ignore all except SIGRTMIN, .. SIGRTMAX
    sigfillset(ignore); 
    sigemptyset(in);

    for (int i = SIGRTMIN; i <= SIGRTMIN + possible_count; ++i) {
        sigaddset(in, i);
    }
}

/*
 * Get signal and info about it
 * using signalfd and its info
 * if SIGRTMIN end programm
 * else print line from file sig_num - SIGRTMIN
 * don't forget that files numerate from zero
 */
static inline void
process_signals(sigset_t *in, FILE **files)
{
    int fd = signalfd(-1, in, 0);
    struct signalfd_siginfo sig_info;
    while (RUN) {
        read(fd, &sig_info, sizeof(sig_info));
	if (sig_info.ssi_signo - SIGRTMIN == 0) {
	    break;
	} else {
	    RDW(files[IGNORE_PROG_NAME(sig_info.ssi_signo - SIGRTMIN)]);
	}
    }
    close(fd);
}

static inline void
close_all(FILE **files, int count)
{
   for (int i = 0; i < count; ++i) {
       fclose(files[i]);
   }
}

int
main(int argc, char *argv[])
{
    //create files and open using names
    FILE * files[argc - 1];

    open_all(files, IGNORE_PROG_NAME(argc), IGNORE_FIRST_NAME(argv));

    //create and set masks to control input
    sigset_t get_sig_mask, ignore_sig_mask;

    set_masks(&get_sig_mask, &ignore_sig_mask, IGNORE_PROG_NAME(argc));

    sigprocmask(SIG_BLOCK, &ignore_sig_mask, NULL);

    //main part to process signals
    process_signals(&get_sig_mask, files);

    //finish line
    close_all(files, IGNORE_PROG_NAME(argc));

    return 0;
}
