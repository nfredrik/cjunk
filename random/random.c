#include<stdio.h>
#include<stdlib.h>
int main()
{
    printf("%x\n",(int)((rand()/(RAND_MAX +1.0))*11));
    printf("%d\n",rand());
return 0;
}
