#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t current_value = 0;
volatile sig_atomic_t pid = 0;
volatile sig_atomic_t must_exit = 0;

// set masks to get only SIGRTMIN
static inline void
set_masks(sigset_t *ignore)
{
    //ignore all except SIGRTMIN, .. SIGRTMAX
    sigfillset(ignore);
    sigdelset(ignore, SIGRTMIN);
}

/*
 * create handler using info 
 * from man 2 sigaction 
 * part with SA_SIGINFO flag
 */
void
handle_sigrtmin(int signum, siginfo_t *info, void *ucontext)
{
    if (info->si_value.sival_int == 0) {
        must_exit = 1;
    } else {
        current_value = info->si_value.sival_int - 1;
        pid = info->si_pid;
    }
}

static inline void
set_sigrtmin_action(struct sigaction *act)
{    memset(act, 0, sizeof(*act));
    act->sa_sigaction = handle_sigrtmin;
    act->sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGRTMIN, act, NULL);
}


int
main()
{
    pid_t pid_read;
    /*
     * create structure to get signals
     */
    struct sigaction sigrtmin_getter;

    set_sigrtmin_action(&sigrtmin_getter);

    union sigval value;
    //create and set masks to control input
    sigset_t ignore_sig_mask;

    set_masks(&ignore_sig_mask);

    sigprocmask(SIG_BLOCK, &ignore_sig_mask, NULL);

    scanf("%d%d", &pid_read, &current_value);

    value.sival_int = current_value;
    sigqueue(pid_read, SIGRTMIN, value);
    while (!must_exit) {
        pause();
        if (must_exit) {
            break;
        } else if (pid > 0) {
	    printf("got: %d\nsend %d\n", current_value+1, current_value); 
            value.sival_int = current_value;
            sigqueue(pid, SIGRTMIN, value);
        }
    }

    return 0;
}

