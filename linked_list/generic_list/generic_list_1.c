#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <iostream>
#include "gen_list.h"
#include "memwatch.h"
/*

 g++ -g generic_list.c  works with gcc now ...

 The data is copied after the the struct ... that's why +1 ....

implement search , sort and so on ...

*/




typedef struct ListDataStruct {
  int femme;
  char  p[24];
  int male;
} list_DATA, *plist_DATA;



void ListDataDestructor( void *p ) {

  // cast the node pointer
  plist pl = (plist)p;
  // cast the data pointer
  plist_DATA pLD = (plist_DATA) ( pl + 1 ); 
  //  delete pLD->p;
  //  free (pLD->p);
}

void TestList()
{

  plist head = NULL;
  plist_DATA nisse;
  plist_DATA k;


	k = (plist_DATA) malloc(sizeof(list_DATA));
	k->femme= 6;
        strcpy( k->p, "lost" );
	k->male=55;
        head = add_2_list( head, (void *) k, sizeof(list_DATA),ListDataDestructor );
        free(k);


	k = (plist_DATA) malloc(sizeof(list_DATA));
	k->femme= 88;
        strcpy( k->p, "nosse" );
	k->male=99;
        head = add_2_list( head, (void *) k, sizeof(list_DATA),ListDataDestructor );
        free(k);


	//        while (NULL != (nisse = (list_DATA*)  get_next_node(head))) {
        while (NULL != (nisse = (plist_DATA)  get_current_node(head))) {
	  printf("string:%s, and femme:%d and male:%d\n", nisse->p, nisse->femme,nisse->male);
	  head = del_node_in_list(head);
	}

        // free the list
        DeleteList( head );
}


main () {

  TestList();
}
