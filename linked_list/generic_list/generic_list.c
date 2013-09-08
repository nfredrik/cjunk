#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "gen_list.h"
/*

 g++ -g generic_list.c

 The data is copied after the the struct ... that's why +1 ....

implement search , sort and so on ...

*/




typedef struct ListDataStruct {
  char  *p;
  int femme;
} list_DATA, *plist_DATA;



void ListDataDestructor( void *p ) {

  // cast the node pointer
  plist pl = (plist)p;
  // cast the data pointer
  plist_DATA pLD = (plist_DATA) ( pl + 1 );
  //  delete pLD->p;
  free (pLD->p);
}

void TestList()
{

  plist head = NULL;
  list_DATA *nisse;
  plist_DATA k;
        plist_DATA d = new list_DATA;
        d->p = new char[24];
        strcpy( d->p, "Hello" );
        head = add_2_list( head, (void *) d, sizeof( plist_DATA ), ListDataDestructor );
        // the object is copied so now delete the original
        delete d;

        d = new list_DATA;
        d->p = new char[24];
        strcpy( d->p, "World" );
        head = add_2_list( head, (void *) d, sizeof( plist_DATA ),ListDataDestructor );
        delete d;


        d = new list_DATA;
        d->p = new char[24];
        strcpy( d->p, "I'm" );
        head = add_2_list( head, (void *) d, sizeof( plist_DATA ),ListDataDestructor );
        delete d;

        d = new list_DATA;
        d->p = new char[24];
        d->femme = 88;
        strcpy( d->p, "lost" );
        head = add_2_list( head, (void *) d, sizeof( plist_DATA ),ListDataDestructor );
        delete d;


	k = (plist_DATA) malloc(sizeof(list_DATA));
        k->p = (char *) malloc(sizeof( char[24]));
	k->femme= 6;
        strcpy( k->p, "lost" );
        head = add_2_list( head, (void *) k, sizeof( plist_DATA ),ListDataDestructor );
        free(k);


	//        nisse = (list_DATA*)  get_current_node(head); 
#if 0
        nisse = (list_DATA*)  get_next_node(head); 
	printf("%s\n", nisse->p);

        nisse = (list_DATA*)  get_next_node(head); 
	printf("%s\n", nisse->p);

	while ( NULL != (nisse = (list_DATA*)  get_next_node(head))) {
	  	  printf("%s\n", nisse->p);
        }

#endif



	//        while (NULL != (nisse = (list_DATA*)  get_next_node(head))) {
        while (NULL != (nisse = (list_DATA*)  get_current_node(head))) {
	  printf("%s, and %d\n", nisse->p, nisse->femme);
	  head = del_node_in_list(head);
	}

        // free the list
        DeleteList( head );
}


main () {

  TestList();
}
