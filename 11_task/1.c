#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
/*
 * read current number
 * must_exit shows terminate command
 * act shows that we got user command, 
 * not terminate
 */
volatile sig_atomic_t current_number = 0;
volatile sig_atomic_t must_exit = 0;
volatile sig_atomic_t action = 0;

void
first_handle(int signum)
{
    action = 1;
    ++current_number;
}

void
second_handle(int signum)
{
    action = 1;
    current_number *= -1;
}

static inline void
set_first(struct sigaction *act)
{
    memset(act, 0, sizeof(*act));
    act->sa_handler = first_handle;
    act->sa_flags = SA_RESTART;
    sigaction(SIGUSR1, act, NULL);
}

void
sigterm_handle(int signum)
{
    must_exit = 1;
}

static inline void
set_second(struct sigaction *act)
{
    memset(act, 0, sizeof(*act));
    act->sa_handler = second_handle;
    act->sa_flags = SA_RESTART;
    sigaction(SIGUSR2, act, NULL);
}

static inline void
set_sigterm(struct sigaction *act)
{
    memset(act, 0, sizeof(*act));
    act->sa_handler = sigterm_handle;
    act->sa_flags = SA_RESTART;
    sigaction(SIGTERM, act, NULL);
}

int
main()
{
    /*
     * create structures to get signals
     */
    struct sigaction sig_first;
    struct sigaction sig_second;
    struct sigaction act_sigterm;

    // fill structures
    set_first(&sig_first);
    set_second(&sig_second);
    set_sigterm(&act_sigterm);

    //print PID
    printf("%d\n", getpid());
    fflush(stdout);

    //get number
    scanf("%d", &current_number);

    //work with signals
    while (!must_exit) {
        pause();
	if (action) {
            printf("%d\n", current_number);
            fflush(stdout);
	    action = 0;
	}
    }
    
    return 0;
}

