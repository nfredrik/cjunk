//#include <linux/timex.h>
#include <stdio.h>
#include <stdlib.h>
//#include <time.h>

main() {

struct timespec fifty_ms;
struct timespec *p;
unsigned long t1, t2;

p = malloc(sizeof(struct timespec));

fifty_ms.tv_sec = 0;
fifty_ms.tv_nsec =(long) 50* 10000000;

t1 = get_cycles();
nanosleep(p, NULL);
t2 = get_cycles();
printf("diff %d\n", t2 - t1);
free(p);

}
