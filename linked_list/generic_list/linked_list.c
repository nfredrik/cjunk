#include <stdio.h>    /* for printf */
#include <stdlib.h>   /* for malloc */
#include "linked_list.h"
 
LLIST *list_add(LLIST **p, int i)
{
	LLIST *n = (LLIST *) malloc(sizeof(LLIST));
	if (n == NULL)
		return NULL;
 
	n->next = *p; /* the previous element (*p) now becomes the "next" element */
	*p = n;       /* add new empty element to the front (head) of the list */
	n->data = i;
 
	return *p;
}
 
void list_remove(LLIST **p) /* remove head */
{
	if (*p != NULL)
	{
		LLIST *n = *p;
		*p = (*p)->next;
		free(n);
	}
}
 
LLIST **list_search(LLIST **n, int i)
{
	while (*n != NULL)
	{
		if ((*n)->data == i)
		{
			return n;
		}
		n = &(*n)->next;
	}
	return NULL;
}
 
void list_print(LLIST *n)
{
	if (n == NULL)
	{
		printf("list is empty\n");
	}
	while (n != NULL)
	{
		printf("print %p %p %d\n", n, n->next, n->data);
		n = n->next;
	}
}
