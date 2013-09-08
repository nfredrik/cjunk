#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

main() {


  FILE *f;
  char *p;

 
  p = getenv("SCULL");


  if(p == NULL) {
    printf("Error: can't get SCULL\n");
    /* fclose(file); DON'T PASS A NULL POINTER TO fclose !! */
    return 1;
  }
  else {

   printf("SCULL: %s\n",p);
  }
  


  f = fopen(p, "r");

 if(f==NULL) {
    printf("Error: can't open file.\n");
    /* fclose(file); DON'T PASS A NULL POINTER TO fclose !! */
    return 1;
  }
  else {
    printf("File opened. Now closing it...\n");
    fclose(f);
    return 0;
  }


}
