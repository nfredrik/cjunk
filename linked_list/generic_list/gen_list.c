
#include "gen_list.h"
#include <stdbool.h>

plist add_2_list( plist head, void * data, size_t datasize,ListNodeDestructor Destructor ) {

plist newlist=NULL;
void *p;

    // Allocate the node and data memory
    newlist = (plist) malloc( datasize + sizeof( list ) );

    // assign a pointer to the data buffer
    p = (void *)( newlist + 1 );

  // copy the data
  memcpy( p, data, datasize );

  newlist->DestructFunc = Destructor;

  // assign the node to the head of the list
  if( head )
  {
       newlist->next = head;
  }
  else
       newlist->next = NULL;
  head = newlist;
  return head;
}


void *get_current_node( plist head )
{

  if (head != NULL)
    return (void *) (head+1);
  else
    return NULL;
}


void *get_next_node( plist head ) {

  static bool first =true;
  static plist h, n;

  if (first) {
    first =false;
    n = head->next;
    return (void *) (head+1);
  }

  h = n;
  if (n != NULL ) {
    n = n->next; 
  }
  else
    return (void *) n;

  return (void *) (h+1);

}


plist del_node_in_list(plist head) {

  plist Next;

  if( head )
    {
      Next = head->next;
      head->DestructFunc( (void *) head );
      free( head );
      head = Next;
    }

  return head;

}


void DeleteList( plist head )
{
        plist Next;
        while( head )
        {
                Next = head->next;
                head->DestructFunc( (void *) head );
                free( head );
                head = Next;
        }
}

