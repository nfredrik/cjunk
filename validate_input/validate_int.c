
#include <stdlib.h>    /* NULL */
#include <limits.h>    /* INT_MAX */

#define MAX_SIZE_OF_STRING ( 100 )
void func( int size )
{
 char* str = NULL;
 
 if(size > 0 && size <= MAX_SIZE_OF_STRING)
 {
  if(str == NULL)
  {
   str = (char*)malloc( size * sizeof(char*));
   if(size < INT_MAX)
   {
    size++;
   }
   /**
   *some code to operate on this string "str"
   **/
  }
  if(str != NULL)
  {
   free(str);
  }
 }
}

main() {

 func(17);

}
