
#include <stdlib.h>
#include <stdio.h>
#include "utlist.h"  /* include el */
//#include "list.h"
#include "memwatch.h"


#define BUFLEN 20

typedef struct el {
  char bname[BUFLEN];
  int foo;
  struct el *next, *prev;
} el;


el *head = NULL; /* important- initialize to NULL! */




/* 
The comparison function must return an int that is negative, zero, or positive, which specifies whether the first item should sort before, equal to, or after the second item, respectively. 

   a  before b   => -1
   a  equal to b => 0
   a  after b    => 1
 */

static int numbercmp(el *a, el *b) {
    return (a->foo > b->foo);
}




int add_number(int num) {

  el *number;

  if ( (number = (el*)malloc(sizeof(el))) == NULL)
    exit(-1);
  
    number->foo =num;
    LL_APPEND(head, number);
 }

int add_pre_number(int num) {

  el *number;

  if ( (number = (el*)malloc(sizeof(el))) == NULL)
    exit(-1);
  
    number->foo =num;
    LL_PREPEND(head, number);
}


void print_sorted() {
#if 1
  el *tmp;
    printf("before sort...\n");
    LL_FOREACH(head,tmp) printf("%d ", tmp->foo);

    LL_SORT(head, numbercmp);
    printf("\nafter sort...\n");
    LL_FOREACH(head,tmp) printf("%d ", tmp->foo);
#endif
}


void get_head_and_del() {
#if 1
  el *nisse;
    printf("head->foo:%d\n", head->foo);
    /* Delete first item */
    nisse = head;
    LL_DELETE(head, head);
    free(nisse);
#endif
}


void delete_all() {

#if 1
  el *tmp, *del = NULL;
    /* Delete all items in the list */
    LL_FOREACH(head,tmp) { 
      if (del) 
	free(del);
      printf("deleting: %d\n", tmp->foo);
      del = tmp;
      LL_DELETE(head, tmp);
    }

    if (del) 
      free(del);
#endif

}


