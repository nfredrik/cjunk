#include <stdio.h>
inline unsigned flipbit ( unsigned x, unsigned bit )
{
  return x ^ (1UL << bit);
}

main()
{
     int v = 0x90; /* a test value */
     int res, i;
     
     printf("\nFlipbit test %0x\n", v);
  
     
     res = flipbit( v, 2 ); /* bit 4 (value 16) inverts to 1->0 or 0->1 */
   
     printf("Hex %0x\n", res ); // result must be: 0x80

}
