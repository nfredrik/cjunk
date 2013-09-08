#include <stdbool.h>

#define MAX_INPUTS 5
#define MAX_OUTPUTS 5
bool digi_in( bool a, bool b) {


}


bool test_all ( int *a, int b) {

  int i;
  bool sum;
  for(i=0;i<MAX_INPUTS;i++) {

    // Shift och maskning av den biten, kan vara ett eller noll
    sum = a[i] & ((b >> i)& 0x01);

  }
  
      return sum;    



}

bool assign_all_do_out(bool *setpointout, int adr) {

  int i;
  int sum;

  for(i = 0; i< MAX_OUTPUTS; i++) {

    sum|= setpointout[i];
   
  }

  // Set all outputs same time
  adr = sum;

  
}

main () {

  int array[MAX_INPUTS];
  int bolle;
  test_all(array, bolle);

}
