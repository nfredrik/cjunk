#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef __OPTIMIZE__
#define __OPT__ 1
#else
#define __OPT__ 0
#endif

#ifdef __OPTIMIZE_SIZE__
#define __OPT_SIZE__ 1
#else
#define __OPT_SIZE__ 0
#endif

#define err_print(args...) __err_print(__FILE__, __FUNCTION__, __LINE__, __DATE__ " " __TIME__, __VERSION__, __OPT__, __OPT_SIZE__, ##args)
void __err_print(char *file, char *function, int line, char *date, char *version, int opt, int opt_size, char *txt, ...)
{
        va_list argp;

        puts("** ERROR! **");
        printf("File:                 \t %s\n",file);
        printf("Function:             \t %s\n",function);
        printf("Line:                 \t %d\n",line);
        printf("Compilation date:     \t %s\n",date);
        printf("Compilator version:   \t %s\n",version);
        printf("Optimization:         \t %s\n",opt==1 ? "Yes" : "No");
        printf("Size optimization:    \t %s\n",opt_size==1 ? "Yes" : "No");

        if (txt == NULL)
                return;
        puts("Description:\n>>>");
        va_start(argp, txt);
        vprintf(txt, argp);
        va_end(argp);
        puts("\n<<<");
}

void other_function() {
        err_print("other %s (%d+%d=%d)", "description",2,2,5);
}

int main() {
        err_print("test %d", 2);
        puts("");
        other_function();
        return 0;
}

