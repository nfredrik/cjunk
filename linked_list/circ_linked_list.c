#include<stdio.h>
//#include<conio.h>
#include<stdlib.h>
#include<malloc.h>
#define null 0
struct node
{
int info;
struct node *link;
}*start;
int main()
{
int ch,n,m,position,i;
struct node *last;
last=null;
while(1)
{
printf("1 create 2  addat 3 addbt  4 del  5 disp 6 exit ");
printf("er ur ch");
scanf("%d",&ch);
switch(ch) {
case 1:
printf("er no of itc");
scanf("%d",&n);
for(i=0;i<n;i++)
{
printf("er the element");
scanf("%d",&m);
create(m);
}break;
case 2:
printf("er the element");
scanf("%d",&m);
addat(m);
break;
case 3:
printf("er the element");
scanf("%d",&m);

printf("er the position");
scanf("%d",&position);
addbt(m,position);
break;
case 4:
if(last==null)
{
printf("list is empty");
continue;
}
printf("er the element for delete");
scanf("%d",&m);
del(m);
break;
case 5:
disp();
break;
case 6:
exit(0);
break;
default:
printf("wrong choice");
}
}

}
create(int data)
{
struct node *q,*tmp, *last;
tmp=(struct node  *)malloc(sizeof(struct node));
tmp->info=data;
tmp->link=null;
if(last==null)
{
last=tmp;
tmp->link=last;
}
else {
tmp->link=last->link;
last->link=tmp;
last=tmp;
}
}


addat(int data)
{

struct node *q,*tmp, *last;
tmp=(struct node  *)malloc(sizeof(struct node));
tmp->info=data;
tmp->link=last->link;
last->link=tmp;
}
addbt(int data,int pos)
{

struct node *tmp,*q, *last;
int i;
q=last->link;;

for(i=0;i<pos-1;i++) {
    q=q->link;
    if(q==last->link) {
    printf("there r lessthan %d elements",pos);
    return;
    }
}


tmp=(struct node  *)malloc(sizeof(struct node));
tmp->link=q->link;
tmp->info=data;
q->link=tmp;
if(q==last)
   last=tmp;
   }
   del(int data)
   {
   struct node *tmp,*q, *last;
   if(last->link==last&&last->info==data)
   {
   tmp=last;
   last=null;
   free(tmp);
   return;
   }
   q=last->link;

   if(q->info==data)
   {
   tmp=q;
   last->link=q->link;
   free(tmp);
   return;
   }
   while(q->link!=last)
   {

   if(q->link->info==data)
   {
   tmp=q->link;
   q->link=tmp->link;
   free(tmp);

   printf("element %d is deleted",data);
   }
  if(q->link->info=data)
  {
  tmp=q->link;
   q->link=last->link;
   free(tmp);
   last=q;
   return;}
   printf("element%d is not found",data);
   }
}

disp()   {
   struct node *q, *last;
   if(last==null)
   {
   printf("list isdempty");
   return;
   }q=last->link;
   while(q!=last)
   {
   printf("%d",q->info);
   q=q->link;
   }
   printf("%d",last->info);
   }
