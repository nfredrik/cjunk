#include <errno.h>
#include <unistd.h>

void read_all_bytes() {

  int len;
  ssize_t ret;
  int fd;
  int buff[1000];
  int *buf;

while (len != 0 && (ret = read (fd, buf, len)) != 0) {
        if (ret == -1) {
                if (errno == EINTR)
                         continue;
                perror ("read");
                break;
        }
        len -= ret;
        buf += ret;
}

}


main() {


}
