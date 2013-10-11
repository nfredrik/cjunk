#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/select.h>

void sighandler(int);

void errExit(char *nisse) 
{
  printf("%s\n", nisse);
  exit(1);
}

int main()
{
  int fd, fd2, n, i;
  fd_set rset;

  struct itimerspec itv, itv2;

  {
    itv.it_value.tv_sec     =  5;
    itv.it_value.tv_nsec    =  0;
    itv.it_interval.tv_sec  =  0;
    itv.it_interval.tv_nsec =  0;

    fd = timerfd_create(CLOCK_REALTIME, 0);
    if (fd == -1)
    errExit("timerfd_create");

    if (timerfd_settime(fd, 0, &itv, NULL) == -1)
      errExit("timerfd_settime");
  }

  {
    itv2.it_value.tv_sec     =  3;
    itv2.it_value.tv_nsec    =  0;
    itv2.it_interval.tv_sec  =  0;
    itv2.it_interval.tv_nsec =  0;

    fd2 = timerfd_create(CLOCK_REALTIME, 0);
    if (fd2 == -1)
    errExit("timerfd_create");

    if (timerfd_settime(fd2, 0, &itv2, NULL) == -1)
      errExit("timerfd_settime");
  }

  FD_ZERO(&rset);
  //FD_CLR(fd, &rset);
  //FD_CLR(fd2, &rset);
  FD_SET(fd, &rset);
  FD_SET(fd2, &rset);

  printf("%d, %d\n", fd, fd2);
  while(1) 
    {

      //printf("1 sec tick!\n");
      // sleep(1);

      //n = select(fd2+1, &rset, NULL, NULL, NULL);
      n = select(fd2+1,&rset, NULL, NULL, NULL);
     
      if (n < 0)
	{
	  errExit("select() returned <0!");
	}

      if (n == 0) {
	printf("timeout has been reached!\n");
	continue;
      }
      

      //      if (FD_ISSET(fd2, &rset))
      int nisse;
      nisse = read(fd2);
      if (nisse > 0)
     {
	      printf ("callback 10 sec timer function!\n");
	      timerfd_settime(fd2, 0, &itv2, NULL);
     }
      else 
     { 
       printf(".");
     }
 
     if (FD_ISSET(fd, &rset))
     {
	      printf ("callback 5 sec timer function!\n");
              //FD_CLR(fd, &rset);
	      timerfd_settime(fd, 0, &itv, NULL);
     }
     

    }

  return(0);
}

