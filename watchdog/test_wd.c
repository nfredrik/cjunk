#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>


main() {

  int status, timeout;
struct watchdog_info ident;

int fd = open("/dev/watchdog", O_WRONLY);

if(fd == -1) {
	perror("open");
	exit(1);
}

int rc = ioctl(fd, WDIOC_GETSUPPORT, &ident);

if(fd == -1) {
	perror("ioctl get support");
	exit(1);
} 

printf("id: %s, fw_version:%d, options:%x\n", ident.identity, ident.firmware_version, ident.options);


rc = ioctl(fd, WDIOC_GETSTATUS, &status);

if(fd == -1) {
	perror("ioctl get_status");
	exit(1);
} 
printf("status:%x\n",status);




rc = ioctl(fd, WDIOC_GETTIMEOUT, &timeout);

if(fd == -1) {
	perror("ioctl get_timeout");
	exit(1);
} 
printf("timeout:%x\n",timeout);


close(fd);
exit(0);

while(1) {

	write(fd,"\0", 1);
	sleep(10);

}



}
