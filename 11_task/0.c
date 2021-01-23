#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t counter = 0;
volatile sig_atomic_t must_exit = 0;

void
sigint_handle(int signum)
{
    ++counter;
}

void
sigterm_handle(int signum)
{
    must_exit = 1;
}

static inline void
set_sigint(struct sigaction *act)
{
    memset(act, 0, sizeof(*act));
    act->sa_handler = sigint_handle;
    act->sa_flags = SA_RESTART;
    sigaction(SIGINT, act, NULL);
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
   struct sigaction act_sigint;
   struct sigaction act_sigterm;
   
   set_sigint(&act_sigint);
   set_sigterm(&act_sigterm);

   printf("%d\n", getpid());
   fflush(stdout);

   while (!must_exit) {
       pause();
   }
   
   printf("%d\n", counter);
}
