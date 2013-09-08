#include <stdio.h>
#include <string.h>


#define COL 1024
#define ROW 1024
#define MAX COL*ROW


#define CHANNELS 128


main() {

FILE *f;

 int row, col, s;
int k = 1;
int p, i, cntr=0;
int left, right;

unsigned int matrix[COL][ROW];
unsigned int *mp;

mp =&matrix;


s = 0;
k =  0;

left  = COL/2-1;
right = COL/2;

memset(&matrix, 0x11, sizeof(matrix));

for(p= 0; p<COL/2; p++) {


#if 1
        for(i= 0; i<COL/2; i++) {

	  if(cntr==1006) 
	    {
	      col = i;
            }

	  if(!(i%2)) {
           /* upper */
	    matrix[CHANNELS*(s+1)-1 -k][left]=cntr++;                // row, col
            row= CHANNELS*(s+1)-1 -k;
	    //	    printf("%d %d\n", row, left);
            matrix[CHANNELS*s +k ][right]=cntr++;
            row =CHANNELS*s +k;
	    //	    printf("%d %d\n", row, right);

         }
         else {

           /* lower */
           matrix[ROW/2+CHANNELS*(s+1)-1 -k][left]=cntr++;
           row = ROW/2+CHANNELS*(s+1)-1 -k;
	   //	    printf("%d %d\n", row, left);
           matrix[ROW/2+CHANNELS*s + k][right]=cntr++;
           row = ROW/2+CHANNELS*s + k;
	   //	    printf("%d %d\n", row, right);

           s++;
           if((i+1)%8 == 0) {
	     s = 0;
	     k += 1;
           }
	   //           if((i+1)%1024 == 0) k += 1;
         }
        }


#endif
        k = 0;
        s = 0;

        /* Going from 0 up to 127 */
#if 1
        for(i= 0; i<COL/2; i++) {

	  if(!(i%2)) {
           /* upper */
            row= CHANNELS/2 -1 -k + CHANNELS*s;
	    matrix[row][left]=cntr++;                // row, col

	    //	    printf("%d %d\n", row, left);
            row= CHANNELS/2  +k + CHANNELS*s ;
            matrix[row][right]=cntr++;

	    //	    printf("%d %d\n", row, right);

         }
         else {

           /* lower */
           row= ROW/2+ CHANNELS/2 -1 -k + CHANNELS*s;
           matrix[row ][left]=cntr++;

	   //	    printf("%d %d\n", row, left);
           row= ROW/2+ CHANNELS/2  +k + CHANNELS*s;
           matrix[row][right]=cntr++;

	   //	    printf("%d %d\n", row, right);

           s++;
           if((i+1)%8 == 0) {
	     s = 0;
	     k += 1;
           }
	   //           if((i+1)%1024 == 0) k += 1;
         }
        }
#endif

	//	printf("k: %d\n", k);	
   left-=1;
   right+= 1;
   k = 0;

}



  f = fopen("unscramble.h","w") ;
  if(f) {
    //    printf("Saving file %d\n", cntr);
    fprintf(f,"#ifndef _UNSCRAMBLE_H\n") ;
    fprintf(f,"#define _UNSCRAMBLE_H\n") ;
    fprintf(f,"#define MAX_NO_OF_PIXELS %d\n", MAX) ;
    fprintf(f,"const unsigned int map[MAX_NO_OF_PIXELS]= {\n") ;
    for(i=0;i<MAX;i++) {
      fprintf(f,"%d,\n",*(mp+i)) ;
    }
    fprintf(f,"};\n") ;
    fprintf(f,"#endif /*_UNSCRAMBLE_H */\n") ;
    fclose(f) ;
  }

#if 0
for (i=0; i<MAX; i++) {

   printf("index:%d:%d\n",i, *(mp+i));

}
#endif

}
