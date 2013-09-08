#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef void (*ListNodeDestructor)( void * );

typedef struct liststruct {
  ListNodeDestructor DestructFunc;
  struct liststruct *next;
  } list, *plist;


/* Add a object to a list pointed out by head. Include data and datasize and Destructor if something need to be purged
   afterwards
*/
plist add_2_list( plist head, void * data, size_t datasize,ListNodeDestructor Destructor );

/*
   Get current node , generic function
*/
void *get_current_node( plist head );


/* 
Get next node each time this routine is called nothing is touched
*/
void *get_next_node( plist head );

/* 
Remove current head node and point to new head node 
*/
plist  del_node_in_list(plist head);


/*
Delete a complete list 
*/
void DeleteList( plist head );


