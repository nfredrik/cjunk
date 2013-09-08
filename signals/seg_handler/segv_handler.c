#define _GNU_SOURCE
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>

#if ZERO
static int segv_handler(int sig)
#endif
static void segv_handler(int sig)
{
	char cmd[100];
	char progname[100];	
	char *p;
	int n;

	n = readlink("/proc/self/exe",progname,sizeof(progname));
	progname[n] = 0;

	p = strrchr(progname, '/');
	*p = 0;
	
	snprintf(cmd, sizeof(cmd), "backtrace %d > /var/log/segv/segv_%s.%d.out 2>&1", 
		 (int)getpid(), p+1, (int)getpid());
	system(cmd);
	signal(SIGSEGV, SIG_DFL);
	//	return 0;
	return ;
}

static void segv_init() __attribute__((constructor));
void segv_init(void)
{
#if ZERO
	signal(SIGSEGV, (sighandler_t) segv_handler);
	signal(SIGBUS, (sighandler_t) segv_handler);
#endif
	signal(SIGSEGV,  segv_handler);
	signal(SIGBUS,   segv_handler);
}
