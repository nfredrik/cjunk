#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/timerfd.h>

void sighandler(int);

int main()
{
  //  signal(SIGPROF, sighandler);
  signal(SIGALRM, sighandler);

  struct itimerspec ts;

  struct itimerval itv;

  itv.it_value.tv_sec     =  5;
  itv.it_value.tv_usec    =  0;
  itv.it_interval.tv_sec  =  0;
  itv.it_interval.tv_usec =  0;

  //int setitimer(int which, const struct itimerval *value, struct itimerval *ovalue);
  //if (setitimer(ITIMER_PROF, &itv, 0) < 0) 
  if (setitimer(ITIMER_REAL, &itv, 0) < 0) 
  {
    printf("failed to set timer!\n");
  }  

  while(1) 
    {
      printf("Going to sleep for a second...\n");
      sleep(1);
    }

  return(0);
}

void sighandler(int signum)
{
  printf("Caught signal %d, coming out...\n", signum);
  exit(1);
}
