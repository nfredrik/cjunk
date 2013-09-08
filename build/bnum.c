#include <stdio.h>

extern char   __BUILD_DATE;
extern char   __BUILD_NUMBER;

main()
{
    printf("Build date  : %u\n", (unsigned long) &__BUILD_DATE);
    printf("Build number: %u\n", (unsigned long) &__BUILD_NUMBER);
}
