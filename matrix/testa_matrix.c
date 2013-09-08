/***************************************************************************
 *   Copyright (C) 2008 by Fredrik Svärd   *
 *   fredrik@dronten   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#define N 2
typedef int matrix[N][N];

void multiply(matrix C, matrix A, matrix B) {

int i, j,k;

for (i=0; i < N; ++i) {
    for (j=0; j < N; ++j) {
        C[i][j] = 0;
        for (k=0; k < N; ++k) {
            C[i][j] += A[k][j] * (B[i][k]);
        }
   }
}

}

void multiply_1(matrix C, matrix A, matrix B) {

int i, j,k;

int acc00, acc01, acc10, acc11;

for (i=0; i < N; i += 2) {
    for (j=0; j < N; j += 2) {
        acc00 = acc01 = acc10 = acc11 = 0;
        for (k=0; k < N; ++k) {
            acc00 += A[k][j+0] * B[i+0][k];
            acc01 += A[k][j+1] * B[i+0][k];
            acc10 += A[k][j+0] * B[i+1][k];
            acc11 += A[k][j+1] * B[i+1][k];
        }
        C[i+0][j+0] = acc00;
        C[i+0][j+1] = acc01;
        C[i+1][j+0] = acc10;
        C[i+1][j+1] = acc11;
    }
}
}


void multiply_2(matrix C, matrix A, matrix B) {

int i, j,k, ii, ib;

int acc00, acc01, acc10, acc11;

for( ii=0; ii < N; ii += ib )
   for( j=0; j < N; j += 2 )
     for( i=ii; i < ii+ib; i += 2 ) {
       acc00 = acc01 = acc10 = acc11 = 0;
       for( k=0; k < N; k++ ) {
         acc00 += A[k][j+0] * B[i+0][k];
         acc01 += A[k][j+1] * B[i+0][k];
         acc10 += A[k][j+0] * B[i+1][k];
         acc11 += A[k][j+1] * B[i+1][k];
       }
       C[i+0][j+0] = acc00;
       C[i+0][j+1] = acc01;
       C[i+1][j+0] = acc10;
       C[i+1][j+1] = acc11;
     }
}

void multiply_3(matrix C, matrix A, matrix B) {

int i, j,k, ii, ib, kk, kb;

int acc00, acc01, acc10, acc11;

 for( ii=0; ii < N; ii += ib )
   for( kk=0; kk < N; kk += kb )
     for( j=0; j < N; j+= 2 )
       for( i=ii; i < ii+ib; i += 2 ) {
         if( kk==0 )
           acc00 = acc01 = acc10 = acc11 = 0;
         else {
           acc00 = C[i+0][j+0];
           acc01 = C[i+0][j+1];
           acc10 = C[i+1][j+0];
           acc11 = C[i+1][j+1];
         }
         for( k=kk; k < kk+kb; k++ ) {
           acc00 += A[k][j+0] * B[i+0][k];
           acc01 += A[k][j+1] * B[i+0][k];
           acc10 += A[k][j+0] * B[i+1][k];
           acc11 += A[k][j+1] * B[i+1][k];
         }
         C[i+0][j+0] = acc00;
         C[i+0][j+1] = acc01;
         C[i+1][j+0] = acc10;
         C[i+1][j+1] = acc11;
       }

}

int main(int argc, char *argv[])
{
  matrix Cet;

  matrix Aet = {1,2,3,4};
  matrix Bet = {{1,2},{3,4}};
  printf("Hello, world!\n");

  printf("Aet[0][0]:%d\n",Aet[0][0]); 
  printf("Aet[0][1]:%d\n",Aet[0][1]); 
  printf("Aet[1][0]:%d\n",Aet[1][0]); 
  printf("Aet[1][1]:%d\n",Aet[1][1]); 
  multiply(Cet, Aet, Bet);

  multiply_1(Cet, Aet, Bet);

 multiply_2(Cet, Aet, Bet);

 multiply_3(Cet, Aet, Bet);


  return EXIT_SUCCESS;
}
