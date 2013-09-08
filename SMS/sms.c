#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

/* Global vars */
char buffer[255];
char* bufptr;
struct timeval timeout;
fd_set input;


void get_response(int fd) {
  int nbytes;
  int n;
  n = select(fd + 1, &input, NULL, NULL, &timeout);
  while(!FD_ISSET(fd, &input)) {}
  bufptr = buffer;
  while((nbytes = read(fd, bufptr, buffer + sizeof(buffer) -
		       bufptr - 1)) > 0) {
    bufptr += nbytes;
    if(bufptr[-1] == '\r' || bufptr[-1] == '\n')
      break;
  }
  *bufptr = '\0';
}


int open_port() {
  int fd;
  fd = open("/dev/ttyb", O_RDWR | O_NOCTTY | O_NDELAY);
  fcntl(fd, F_SETFL, 0);
  return(fd);
}

void set_options(int fd) {
  struct termios options;
  /* Get current options */
  tcgetattr(fd, &options);
  /* Set raw input, 1 sec timeout */
  options.c_cflag |= (CLOCAL | CREAD);
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_oflag &= ~OPOST;
  options.c_cc[VMIN] = 0;
  options.c_cc[VTIME] = 10;
  /* Set baudrate */
  cfsetispeed(&options, B19200);
  cfsetospeed(&options, B19200);
  /* Set new options */
  tcsetattr(fd, TCSANOW, &options);
}

void send_string(int fd, char* to_send, int num) {
  int res;
  do {
    res = write(fd, to_send, num);
  } while(res == -1 && errno == EINTR);
  sleep(1);
}

int string_parser(char *str, char *substr) {

  return 1;
}
void init_modem(int fd) {
  send_string(fd, "AT\r", 3);
  get_response(fd);
  bufptr = buffer;
  if(string_parser(bufptr, "OK"))
    ;/* Everything okay... */
  else
    ;/* Something wrong */

    send_string(fd, "AT+CMGF=1\r", 10);
  get_response(fd);
  bufptr = buffer;
  if(string_parser(bufptr, "OK"))
    ;/* Everything okay... */
  else
    ;/* Something wrong */
}


main() {

  int my_fd;
  my_fd = open_port();
  set_options(my_fd);
  FD_ZERO(&input);
  FD_SET(my_fd, &input);
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;


  send_string(my_fd, "AT+CMGS=\"+46709359396\"\r", 23);
  get_response(my_fd);       //invänta prompten från modemet
  send_string(my_fd, "Testing testing! No reply please...\x1A", 36);
  get_response(my_fd);


}
