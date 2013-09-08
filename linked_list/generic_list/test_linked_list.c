
#include <stdio.h>    /* for printf */
#include "linked_list.h"
 
int main(void)
{
	LLIST *n = NULL;
 
	list_add(&n, 0); /* list: 0 */
	list_add(&n, 1); /* list: 1 0 */
	list_add(&n, 2); /* list: 2 1 0 */
	list_add(&n, 3); /* list: 3 2 1 0 */
	list_add(&n, 4); /* list: 4 3 2 1 0 */
	list_print(n);
	list_remove(&n);                 /* remove first (4) */
	list_remove(&n->next);           /* remove new second (2) */
	list_remove(list_search(&n, 1)); /* remove cell containing 1 (first) */
	list_remove(&n->next);           /* remove second to last node (0) */
	list_remove(&n);                 /* remove last (3) */
	list_print(n);
 
	return 0;
}

