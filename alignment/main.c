#include <stdio.h>
#include <stddef.h>

struct foo_o {
       char     a;
       short    b;
       int      c;
}__attribute__ ((packed));

struct foo {
       unsigned int  a;
       unsigned char   b;
       unsigned char    c;
}__attribute__ ((packed));

#define OFFSET_A        offsetof(struct foo, a)
#define OFFSET_B        offsetof(struct foo, b)
#define OFFSET_C        offsetof(struct foo, c)

int main ()
{
        printf ("offset A = %d\n", OFFSET_A);
        printf ("offset B = %d\n", OFFSET_B);
        printf ("offset C = %d\n", OFFSET_C);
        printf ("sizeof foo = %d\n", sizeof(struct foo));
        return 0;
}
