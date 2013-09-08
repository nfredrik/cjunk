
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned int gen_cmd_tag() {

  return (rand() % 0xffffffff);
}


int main ()
{

  /* STEP ONE */
  /* initialize random generator */
  srand ( time(NULL) );

  /* generate random numbers */
  printf("RAND_MAX = ", RAND_MAX);
  
  printf ("A number between 0 and RAND_MAX : %d\n", rand());
  
  printf ("A number between 0 and 99: %d\n", rand() % 100);
  
  printf ("A number between 0 and 9: %d\n", rand() % 10);

  printf ("A number between 0 and 0xffff: %x\n", rand() % 0xffffffff);


  /* Call gen_cmd_tag */

  printf ("A number between 0 and 0xffff: %x\n", gen_cmd_tag());

  return 0;
}
