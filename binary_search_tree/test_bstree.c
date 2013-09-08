/*
 *  GPL
 *
 *  Written by Diogo Sousa aka orium
 *  Copyright (C) 2008 Diogo Sousa
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <stdio.h>
#include <assert.h>
 
#include "bstree.h"
 
struct integer
{
	int n;
	char *english;
};
 
int my_cmp(const void *p1, const void *p2)
{
	return ((struct integer *)p1)->n-((struct integer *)p2)->n;
}
 
void my_twalk_action(void *data)
{
	printf("%d %s\n",((struct integer *)data)->n,
	       ((struct integer *)data)->english);
}
 
int main(void)
{
	struct bstree_node *bstree_root=NULL;
	struct integer i1={1,"one"};
	struct integer i2={2,"two"};
	struct integer i3={3,"three"};	
	struct integer i4={4,"four"};
	struct integer i5={5,"five"};
	struct integer lost={2,NULL};
	struct integer *needle;
 
	bstree_add(&bstree_root,&i3,my_cmp);
	bstree_add(&bstree_root,&i1,my_cmp);
	bstree_add(&bstree_root,&i5,my_cmp);
	bstree_add(&bstree_root,&i4,my_cmp);
	bstree_add(&bstree_root,&i2,my_cmp);
 
	bstree_walk(bstree_root,my_twalk_action);
 
	needle=bstree_search(bstree_root,&lost,my_cmp);
 
	assert(needle != NULL);
 
	printf("%s\n",needle->english);
 
	bstree_free(bstree_root,NULL);
 
	return 0;
}
