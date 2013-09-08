#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "utlist.h"  /* include el */
#include "list.h"  /* include el */




/*
 gcc -g -o test -DMEMWATCH -DMEMWATCH_STDIO  memwatch.c single_linked.c

 gcc -g -o test -DMEMWATCH -DMEMWATCH_STDIO  memwatch.c single_linked.c list.c

*/



int main(int argc, char *argv[]) {

  //    char linebuf[BUFLEN];

    add_number(1);
    add_number(13);
    add_number(8);
    add_number(8);
    get_head_and_del();
    add_number(8);
    add_number(91);
    add_number(32);

    add_pre_number(19);

    get_head_and_del();

    print_sorted();



    delete_all();



    return 0;
}
