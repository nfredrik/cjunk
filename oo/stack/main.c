#include <stdio.h>
#include <string.h>
#include "stack.h"

struct s {
      int nisse;
      char olle[5];
};

main () {

   struct s  najma; 

  Stack <struct s> test;

  Stack <int> inst;
  
  inst.Push(40);
  inst.Push(4);
  inst.Push(4);
  inst.Push(41);
  printf("%d\n", inst.Pop());
  inst.Peek(1);


  /* Test */

  najma.nisse = 5;
  strcpy(najma.olle, "hello");
  test.Push(najma);
  test.Push(najma);
  test.Push(najma);
  printf("%d\n", test.Pop().nisse);
  printf("%s\n", test.Pop().olle);
}
