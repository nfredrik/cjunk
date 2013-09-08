#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* minimum required number of parameters */
#define MIN_REQUIRED 2

/* display usage */
int help() {
   printf("Usage: myprogram [-s <arg0>] [-n <arg1>] [-true]\n");
   printf("\t-s: a string a\n");
   printf("\t-n: a number\n");
   printf("\t-true: a single parameter\n");

   return 1;
}

/* main */
int main(int argc, char *argv[]) {
   if (argc < MIN_REQUIRED) {
      return help();
   }
   int i;

   /* iterate over all arguments */
   for (i = 1; i < (argc - 1); i++) {
       if (strcmp("-s", argv[i]) == 0) {
          /* do something with it */ 
          printf("string = %s\n", argv[++i]);
          continue;
       }
       if (strcmp("-n", argv[i]) == 0) {
          /* do something with it. for example, convert it to an integer */
          printf("number = %i\n", atoi(argv[++i]));
          continue;
       }
       if (strcmp("-true", argv[i]) == 0) {
          printf("true activated\n");
          continue;
       }
       return help();
   }
   return 0;
}

