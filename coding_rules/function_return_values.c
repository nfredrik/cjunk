
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

/*---------------------------------------------------------------------------*/
/* add_work()

This  an imperative command, the function should return: 

 an error-code integer, on failure  eg -EBUSY
 zero on success!

*/
/*---------------------------------------------------------------------------*/
int add_work(void) {

  /* Use at failure */
  return -EBUSY;

  /* Use at success */
  return 0;
}

/*---------------------------------------------------------------------------*/
/* pci_dev_present()


This a predicate, the function should return: 

 false == 0, on failure
 true == 1, on success
*/
/*---------------------------------------------------------------------------*/

bool pci_dev_present() {

  /* Use at failure */
  return false;

  /* Use at success */
  return true;
}

/*---------------------------------------------------------------------------*/
/* Use arguments to pass pointers and other stuff instead of return values ! */
/*---------------------------------------------------------------------------*/

main() {


  /* _Imperative_ or _Action_ function */
  if(add_work()) {
    printf("We failed for add_work!\n");
  }
  else {
    printf("Success for for add_work!\n");
  }



    /* The "PCI device present" is a _predicate_ true if success, false if failure!*/
    if (pci_dev_present()) {
      printf("pci_dev_present is present !!!\n");
    }
    else {
      printf("pci_dev_present is _NOT_ present !!!\n");
    }

}
