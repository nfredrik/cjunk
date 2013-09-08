 
typedef struct node {
	int data;
	struct node *next; /* pointer to next element in list */
} LLIST;
 
LLIST *list_add(LLIST **p, int i);

void list_remove(LLIST **p);

LLIST **list_search(LLIST **n, int i);

void list_print(LLIST *n);
