/* sig_talk.c --- Example of how 2 processes can talk */
/* to each other using kill() and signal() */
/* We will fork() 2 process and let the parent send a few */
/* signals to it`s child  */

/* cc sig_talk.c -o sig_talk  */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>


void sighup(); /* routines child will call upon sigtrap */
void sigint();
void sigquit();
void sigkill();

/* Obtain a backtrace and print it to stdout. */
void
print_trace (void)
{
void *array[10];
size_t size;
char **strings;
size_t i;

size = backtrace (array, 10);
strings = backtrace_symbols (array, size);

printf ("Obtained %zd stack frames.\n", size);

for (i = 0; i < size; i++)
printf ("%s\n", strings[i]);

free (strings);
}

main()
{ int pid;

  /* get child process */
  
   if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    }
    
   if (pid == 0)
     { /* child */
       signal(SIGHUP,sighup); /* set function calls */
       signal(SIGINT,sigint);
       signal(SIGQUIT, sigquit);
//       signal(SIGKILL, sigkill);
       for(;;); /* loop for ever */
     }
  else /* parent */
     {  /* pid hold id of child */
       printf("\nPARENT: sending SIGHUP\n\n");
       kill(pid,SIGHUP);
       sleep(3); /* pause for 3 secs */
       printf("\nPARENT: sending SIGINT\n\n");
       kill(pid,SIGINT);
       sleep(3); /* pause for 3 secs */
       printf("\nPARENT: sending SIGQUIT\n\n");
       kill(pid,SIGQUIT);
//       kill(pid,SIGKILL);
       sleep(3);
     }
}

void sighup()

{  signal(SIGHUP,sighup); /* reset signal */
   printf("CHILD: I have received a SIGHUP\n");
}

void sigint()

{  signal(SIGINT,sigint); /* reset signal */
   printf("CHILD: I have received a SIGINT\n");
}

void sigquit()

{ 
  print_trace();
  printf("My DADDY has Killed me!!!\n");
  exit(0);
}

// It seems that SIGKILL dont call this routine
void sigkill()
{ 
  print_trace();
  printf("My DADDY has murded me!!!\n");
  exit(0);
}

