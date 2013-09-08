#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>



 
int main(void)
{
    time_t     now;
    struct tm  *ts;
    char       buf[80];
    FILE *fd;
    char *s;
 
    // Get the current time
    time(&now);
    s = malloc(80);
 
    // Format and print the time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = localtime(&now);
//    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
    strftime(buf, sizeof(buf), "%s", ts);
    sprintf(s, "%s", buf);
    fd = fopen(s, "wb");
//    fd = open(s, O_WRONLY);
    
    if(fd == NULL) {
      printf("We failed to open: %s errno: %s \n",s, strerror(errno));
      exit(1);        
    }
        
    if (fclose(fd) == EOF ) {
      printf("We failed to close: %s errno: %s \n",s, strerror(errno));
    }
    free(s);
 
    return 0;
}
