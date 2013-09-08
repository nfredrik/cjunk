#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "communication.h"
#include "cs_commands.h"

#define WITH_WATCH_DOG    0
#define TIMEOUT_APPL      5
#define MAX_CONN_ATTEMPTS      5
#define SENT_ECHOS        3
#define LOCAL_HOST "127.0.0.1"
#define PEER_CLOSED  0xff
#define YES 1
#define NO  0
FILE *outfile;

#line 5

typedef struct {

  pid_t appl_pid;
  int gotit;
  int wait_cntr;
  int cmd_tag;
} conn_status_t;

conn_status_t conn_status;

int clientSocket = -1;

int watchdog;

fd_set rset, wset;


void stop_watchdog();



void start_application() {

  int status;

  printf("trying to start application\n");

  if(!fork())
    execl("/bin/date", "date", NULL);
  else {
  conn_status.appl_pid = wait(&status);
    printf("with the program date\n");
  }
}

void kill_application() {

  if(kill(conn_status.appl_pid,SIGKILL ))
     perror("kill failed");
}


void close_connection() {

  close(clientSocket);
  clientSocket = -1;
}

/*
**===========================================================================
** 8.0			start_watchdog()
**===========================================================================
** Description: 
**
** Parameters:  
**              
** Returns:     none
**
** Globals:     rset
*/

void start_watchdog() {

  int status, timeout;
  struct watchdog_info ident;

  if((watchdog =open("/dev/watchdog", O_WRONLY)) == -1)
    printf("start_watchdog, open failed: %s\n", strerror(errno));


  if(ioctl(watchdog, WDIOC_GETSUPPORT, &ident) == -1)
    printf("start_watchdog, ioctl1 failed: %s\n", strerror(errno));
  else
    printf("start_watchdog id: %s, fw_version:%d, options:%x\n", ident.identity, ident.firmware_version, ident.options);


  if(ioctl(watchdog, WDIOC_GETSTATUS, &status))
    printf("start_watchdog, ioctl2 failed: %s\n", strerror(errno));
  else
    printf("start_watchdog status:%x\n",status);


  if(ioctl(watchdog, WDIOC_GETTIMEOUT, &timeout))
    printf("start_watchdog, ioctl3 failed: %s\n", strerror(errno));
  else   
    printf("start_watchdog timeout:%x\n",timeout);


  // FIXME: need to set watchdog timeout to 10 or 30 secs or what?
  // Double or tripple the TIMEOUT_APPL * SEND_ECHOS?


}
      

void kick_watchdog() {

  if(write(watchdog,"\0", 1) == -1)
    printf("kick_watchdog, write failed: %s\n", strerror(errno));

}

void stop_watchdog() {

  // FIXME : is this enough or will the watchdog restart the system now?
  if(close(watchdog) == -1)
    printf("close_watchdog, close failed: %s\n", strerror(errno));
}

/*
**===========================================================================
** 8.0			catch_int()
**===========================================================================
** Description: 
**
** Parameters:  
**              
** Returns:     none
**
** Globals:     rset
*/

void catch_int(int sig_num)
{
  /* re-set the signal handler again to catch_int, for next time */
  signal(SIGINT, catch_int);

#if WITH_WATCH_DOG
  stop_watchdog();
#endif
  if (clientSocket != -1) {
    close_connection();
  }
  exit(1);
}

void wait_for_reboot() {
     while (1) {}
}


/*
**===========================================================================
** 8.0			catch_int()
**===========================================================================
** Description: 
**
** Parameters:  
**              
** Returns:     none
**
** Globals:     rset
*/


void connect_2_appl() {

  int i = 0;

  /* Return if we still have a connection */

  if (clientSocket != -1) return;

  while(1) {

    if ((clientSocket = createConnection(SERVER_PORT, (char*)LOCAL_HOST, 0, outfile)) != -1) {
      break;   /* We got a connection! */
    }
  
    if( MAX_CONN_ATTEMPTS < i++) {
      printf("We failed to connect to application, wait for a system restart!\n");
#if TEST_APPL
      kill_application();
      /* Restart application, how? When do go for a restart of the system?*/
      start_application();
#endif
      /* Max number of attempts reached, stop kick watchdog and wait for a reboot */
      wait_for_reboot();
    }

#if WITH_WATCH_DOG
    /* Kick the watchdog, sleep for a while and make a new attempt */
    kick_watchdog();
#endif
    sleep(TIMEOUT_APPL); 
  }



}

/*
**===========================================================================
** 8.0			build_select_list()
**===========================================================================
** Description: Build a fd_set of sockets that select() will watch... 
**
** Parameters:  
**              
** Returns:     none
**
** Globals:     rset
*/
void build_select_list()
{
  /* FD_ZERO() clears out the fd_set called rset, so that
     it doesn't contain any file descriptors. */
  FD_ZERO(&rset);
  //  FD_ZERO(&wset);


  /* Put the clientSocket in both read and write set ... hope this works :-)*/

  FD_SET(clientSocket,&rset);	
  //  FD_SET(clientSocket,&wset);	
}


/*
**===========================================================================
** 8.0			receive_cmd()
**===========================================================================
** Description: Receives a command from the Control system
**              
**
** Parameters:  clientSocket
**              
**
** Returns:     msg - the message
**
** Globals:     in_buf[]
*/
Cmd *receive_cmd(int clientSocket) 
{

  static Cmd msg;

  //  static int8u command;

  int len;
  int sum = 0; 
  ssize_t y;
  char marker;
  unsigned short sm_inbuf[512];
    

  /* Get Marker */
  do {
    len = recv(clientSocket, &marker, 1, 0);

    if ( 0 >= len ) {

      if( 0 > len ) {
	if (errno == EWOULDBLOCK) {
	  printf("receive_msg: timeout\n");
	}
	else
	  perror("receive_msg, marker");	 // FIXME: better logging
      }

      /* Connection closed, close this end */
      printf("\nConnection lost: FD=%d\n", clientSocket);
      close_connection();
      msg.cmd = PEER_CLOSED;
      return (Cmd *) &msg;

    }
    if (marker != CS_MARKER){ printf("no marker yet, got %x \n", marker);} 
  } while((len >=0) && (marker != CS_MARKER)) ;


  /* Get Header */
  if( (len = recv(clientSocket, &msg, sizeof(Cmd), 0)) == -1) {
    perror("wrong in message") ;
  }

  if(msg.length > 0) { 

    while(sum < msg.length) {
      if( (y = recv(clientSocket,&sm_inbuf[sum], (msg.length-sum), 0)) == -1) {
    	perror("recv data") ;
      }
      sum+= y;
    }
    return (Cmd *) &msg;
  }
  return (Cmd *) &msg;

}
  /*
  **===========================================================================
  ** 8.0			send_cmd()
  **===========================================================================
  ** Description: Sends a command to the Control system
  **              
  **
  ** Parameters:  clientSocket
  **              msg
  **
  ** Returns:     none
  **
  ** Globals:     
  */
  int send_cmd(int clientSocket, Cmd *msg) 
  {
    int ss;
    int8u pmarker = IRM_MARKER;
    unsigned short sm_outbuf;
    /* Send marker */
    if ( (ss=send( clientSocket, &pmarker,  1, 0 )) <= 0 ) {
	perror("System says..");
	return -1;
    }

    /* Send Header */
    if ( (ss=send( clientSocket, msg, sizeof(Cmd), 0 )) <= 0 ) {
	perror("System says..");
	return -1;
    }

    if (msg->length > 0) {
      if ( (ss=send( clientSocket, &sm_outbuf, msg->length, 0 )) <= 0 ) {
	  perror("System says..");
	return -1;
      }
    }
    return 0;
  }

  /*
  **===========================================================================
  ** 8.0			recv_another_command()
  **===========================================================================
  ** Description: Check the reply from the application
  **
  ** Parameters:  
  **              
  ** Returns:     
  **
  ** Globals:     
  */
  int recv_another_command(int cmd_tag) {

    Cmd *reply;
    int ret = -1;

    reply = receive_cmd(clientSocket);

    switch (reply->cmd) {

    case CMD_ECHO_CS_REPLY:
      printf("Got CMD_ECHO_CS_REPLY\n");
      // FIXME: check commando tag...
      if (reply->cmd_tag != cmd_tag) {
      printf("cmd_tag differs!!\n");
      }
      /* Check result code */
      reply->result;
      ret = 0;
      break;

    case PEER_CLOSED:
      printf("Application closed!!\n");
      // FIXME: try to connect again ...
      ret = -1;
      break;

    default:
      printf("Got something else: %x \n", reply->cmd );
      // FIXME: what to do if not echo??     
      ret = -1;
      break;
    }

    return ret;
  }


  /*
  **===========================================================================
  ** 8.0			send_another_command()
  **===========================================================================
  ** Description: Build a fd_set of sockets that select() will watch... 
  **
  ** Parameters:  
  **              
  ** Returns:     none
  **
  ** Globals:     rset
  */

  int send_another_command(int cmd_tag) {
  
    Cmd echo;
    //    Cmd *p = &echo;

    memset( &echo, 0, sizeof(Cmd));
    echo.version = PROTOCOL_VERSION;
    echo.cmd = CMD_ECHO_CS;
    echo.option = echo.result = 0;
    echo.cmd_tag = cmd_tag;
    echo.length = 0;

    /* Send command to the application */
    return send_cmd(clientSocket, &echo);
  }



/*
**===========================================================================
** 		main()
**===========================================================================
** Description: The system monitor implements a surveillance function. 
**              It will launch and kick a hardware watchdog. The implementation
**              could differ between the mother boards and lead to changes
**              in this implementation.
**
**              The main task is to keep the application and system alive.
**                   
*/            
int  main () {

  int no_of_sockets_ready_to_read;
  struct timeval select_timeout;  /* Timeout for select */

  outfile =stderr;
  conn_status.gotit = NO;
  conn_status.wait_cntr = 0;
  conn_status.cmd_tag = 0x37;

  /* Perhaps should the application be started by the monitor? */
#if TEST_APPL
  start_application();
#endif
  /* set the INT (Ctrl-C) signal handler to to be able terminate in a nice way */
  signal(SIGINT, catch_int);


  /* Nota Bene: the device file have to be created with:
     mknod -m 666 /dev/watchdog c 10 130 */

#if WITH_WATCH_DOG
  /* Open a connection to the hardware watchdog. Configure it if needed */
  start_watchdog();

  /* Start kick the watchdog to avoid a restart of the system */
  kick_watchdog();
#endif 

  /* Connect to the data acquisition application. We will not return from
     this routine if we fail, i.e we go for a restart */
  connect_2_appl();

  /* Send the initial echo to the application, don't care about errors this time :-) */
  if( send_another_command(conn_status.cmd_tag) == -1)
     printf("First echo to application failed!\n");

  while (1) {


    /* Build a select() list both for read and write */
    build_select_list();
    
    /* Define timeout for select(). Initiate every time since it could be modified
       by kernel at select() */
    select_timeout.tv_sec = TIMEOUT_APPL;
    select_timeout.tv_usec = 0;

    no_of_sockets_ready_to_read = select(clientSocket+1, &rset,(fd_set *) 0 , 
                                         (fd_set *) 0,  &select_timeout);
		
    if (no_of_sockets_ready_to_read < 0) {
      perror("select");
      // FIXME: do we need to restart?
    }

    /* We reached timeout */
    if (no_of_sockets_ready_to_read == 0) {

#if WITH_WATCH_DOG
      /* Kick the watchdog */
      kick_watchdog();
#endif
      /* We got a reply recently ... */
      if (conn_status.gotit == YES) {
        conn_status.gotit = NO;
	conn_status.wait_cntr = 0;
        if (send_another_command(conn_status.cmd_tag) == 0) {

	  printf("select(), everything is OK :-)\n");
	}
	else {
	  /* We failed to send a echo, should we close connection  and
	     connect again? */
          close_connection();
	  connect_2_appl();
	}

      }
      /* No reply from application yet ... */
      else {
      
	/* We waited TIMEOUT_APPL * SEND_ECHOS, close connection and try again */
	if (conn_status.wait_cntr++ > SENT_ECHOS) {
          conn_status.wait_cntr = 0;
          close_connection();
	  connect_2_appl();
	}
      }
    }
    else {

      /* Reply from application? */
      if (FD_ISSET(clientSocket, &rset))
	if(recv_another_command(conn_status.cmd_tag) == 0) {
	  conn_status.gotit = YES; 
	}
        else
          /* If the application initiated a close() we no connection  now */
	  connect_2_appl();

      else
	printf("select(), how did this happen?\n");
    }
  }
  return 0;
}
