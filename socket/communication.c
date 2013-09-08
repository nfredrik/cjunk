/*
	                     *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : Gemini                                                      **
** filename :  interface.c                                                 **
** version   : 1                                                           **
** date      : September 18, 2007                                          **
**                                                                         **
*****************************************************************************
**                                                                         **
** Copyright (c) 2007, C-RAD Imaging                                       **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1
Date        : September 18, 2007
Revised by  : Fredrik Svärd
Description : Original version.

        CONTENTS
        --------

        1  Description
        2  History of development
        3  Include files
           3.1 Blockwide definitions
           3.2 Libraries
           3.3 Modulewide definitions
           3.4 Global signals
           3.5 Local signals
        4  Local definitions
           4.1 Manifest constants
           4.2 Type definitions
           4.3 Macros
           4.4 Signal composition
        5  External references
           5.1 Functions
           5.2 Variables
           5.3 Forward references
           5.4 Processes
        6  Global variables
        7  Local variables
           7.1 General variables
           7.2 Receive specifications
        8  Local functions
        9  Global functions
        10 Process entrypoints

*****************************************************************************
*/

/*
*****************************************************************************
** 1  DESCRIPTION.
*****************************************************************************
** This file is part of the TIPR module.
**
** This file contains the tip_stack's subfunction called overload supervision
** With overload supervision it will be possible to avoid restarts in the RP.
** 
*/

/*
*****************************************************************************
** 2  HISTORY OF DEVELOPMENT.
*****************************************************************************
** date      responsible           notes
** --------  --------------------  ------------------------------------------
** 07-10-30  Fredrik Svärd         Initial version
** 080708    Fredrik Svärd         Added LOG2FILE

TODO : - replace errror() and so on with log4c logging...
       - remove non used include files ...
*/

/*
*****************************************************************************
** 3  INCLUDE FILES.
*****************************************************************************
*/

/*
**===========================================================================
** 3.1  Blockwide definitions.
**===========================================================================
*/

/*
**===========================================================================
** 3.2  Libraries.
**===========================================================================
*/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "communication.h"


/*
**===========================================================================
** 3.3  Modulewide definitions.
**===========================================================================
*/

/*
*****************************************************************************
** 4  LOCAL DEFINITIONS.
*****************************************************************************
*/

/*
**===========================================================================
** 4.1  Manifest constants.
**===========================================================================
*/

/*
**===========================================================================
** 4.2  Type definitions.
**===========================================================================
*/

/*
**===========================================================================
** 4.3  Macros.
**===========================================================================
*/
#define MAX_SERVCLIENTS  (5)
#define SOCKET_TIMEOUT    3    /* Both send() and recv() */

#define GIGABIT_ENET 1000000000   /* bits per second */
#define FAST_ENET     100000000
#define LINK_BANDWITDH FAST_ENET
#define RTT  0.050                /* seconds */
#define BDP  (int)(LINK_BANDWITDH * RTT)/8  /* Bandwidth Delay Product in bytes*/

/*
**===========================================================================
** 4.4  Signal composition.
**===========================================================================
*/

/*
*****************************************************************************
** 5  EXTERNAL REFERENCES.
*****************************************************************************
*/

/*
**===========================================================================
** 5.1  Functions.
**===========================================================================
*/
#define _COMMUNICATION_C_SRC

/*
**===========================================================================
** 5.2  Variables.
**===========================================================================
*/

/*
**===========================================================================
** 5.3  Forward references.
**===========================================================================
*/

/*
**===========================================================================
** 5.4  Processes.
**===========================================================================
*/

/*
*****************************************************************************
** 6  GLOBAL VARIABLES.
*****************************************************************************
*/

/*
*****************************************************************************
** 7  LOCAL VARIABLES.
*****************************************************************************
*/

/*
**===========================================================================
** 7.1  General variables.
**===========================================================================
*/
/*
**===========================================================================
** 7.2 Receive specifications.
**===========================================================================
*/

/*
*****************************************************************************
** 8  LOCAL FUNCTIONS.
*****************************************************************************
*/

/*
**===========================================================================
** 8.0			createConnection()
**===========================================================================
** Description: Create a TCP connection on client side
**
** Parameters:  port
**              addr
**              bufsize - on recv and send buffer
**              
**
** Returns:     returns created socket
**
** Globals:     
*/
int createConnection(int port, char *addr, int bufsize)
{
  struct sockaddr_in serverSockAddr;
  int clientSocket, size;
  int one = 1; 
  bzero( &serverSockAddr, sizeof( serverSockAddr ));

  serverSockAddr.sin_family      = AF_INET;
  serverSockAddr.sin_port        = htons( port );
  serverSockAddr.sin_addr.s_addr = inet_addr( addr );
  //  serverSockAddr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP

  /*---------------*/                                            /* SOCKET() */
  if ( (clientSocket = socket( AF_INET, SOCK_STREAM, 0 )) == -1 )
    {

    }

  /*---------------*/                                           /* CONNECT() */
  if ( connect( clientSocket, (struct sockaddr *)&serverSockAddr, 
                sizeof( serverSockAddr ) ) != 0 )
    {

 
      clientSocket = -1;
    }


  /*-----------------------*/                                /* SETSOCKOPT() */
  if (setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR,
                (char *)&one, sizeof(one) ) < 0)
  {

    clientSocket = -1;
  }

  /*---------------*/                                        /* SETSOCKOPT() */
  if (bufsize > 30000)
  {  
     size= bufsize;
     /******************
     setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF,
               (char *) &size, sizeof (size));
     setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF,
               (char *) &size, sizeof (size));
     ****************/
  }  

  return( clientSocket );
} /* createConnection() */


/*
**===========================================================================
** 8.0			createUDPConnection()
**===========================================================================
** Description: Create a UDP connection on client side
**
** Parameters:  port
**              addr
**              bufsize - on recv and send buffer
**              
**
** Returns:     returns created socket
**
** Globals:     
*/
int createUDPConnection(int port, char *addr, int bufsize)
{
  struct sockaddr_in serverSockAddr;
  int clientSocket;
  bzero( &serverSockAddr, sizeof( serverSockAddr ));

  serverSockAddr.sin_family      = AF_INET;
  serverSockAddr.sin_port        = htons( port );
  serverSockAddr.sin_addr.s_addr = inet_addr( addr );
  //  serverSockAddr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP

  /*---------------*/                                            /* SOCKET() */
  if ( (clientSocket = socket( AF_INET, SOCK_DGRAM, 0 )) == -1 )
    {

      return clientSocket; 
    }

 return clientSocket;

} /* createUDPConnection() */



/*
**===========================================================================
** 8.1			createInternalConnection()
**===========================================================================
** Description: Create a Unix domain connection on client side
**
** Parameters:  addr    - file to use
**              
**
** Returns:     returns created socket
**
** Globals:     
*/
int createInternalConnection(char *addr)
{
  struct sockaddr_un serverSockAddr;
  int clientSocket;

  bzero( &serverSockAddr, sizeof( serverSockAddr ));

  serverSockAddr.sun_family      = AF_UNIX;
  strncpy(serverSockAddr.sun_path, addr,sizeof(serverSockAddr.sun_path));

  /*---------------*/                                            /* SOCKET() */
    if ( (clientSocket = socket( PF_UNIX, SOCK_STREAM, 0 )) == -1 )
    {

    }

  /*---------------*/                                           /* CONNECT() */
  if ( connect( clientSocket, (struct sockaddr *)&serverSockAddr, 
                sizeof( serverSockAddr ) ) != 0 )
    {
      clientSocket = -1;

    }



  return( clientSocket );

} /* createInternalConnection() */


/*
**===========================================================================
** 8.1			create_service()
**===========================================================================
** Description: Create a TCP listening socket of server side
**
** Parameters:  port
**              bufsize - sizeof recv and send buffer
**              nodelay - to use or not  
**              
**
** Returns:     returns created socket
**
** Globals:     
*/
int create_service(unsigned short port, int bufsize, int nodelay)
{

  struct sockaddr_in serverSockAddr;
  int serverSocket, size;
  socklen_t optlen = 0;
  int one = 1; 
 

  bzero( &serverSockAddr, sizeof( serverSockAddr ));

  serverSockAddr.sin_family      = AF_INET;
  serverSockAddr.sin_port        = htons( port );
  serverSockAddr.sin_addr.s_addr = htonl ( INADDR_ANY );

  /*-----------------------*/                                    /* SOCKET() */
  if ( (serverSocket = socket( AF_INET, SOCK_STREAM, 0 )) == -1 )
    {

    }


  /*-----------------------*/                                /* SETSOCKOPT() */
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,
                (char *)&one, sizeof(one) ) < 0)
  {

  }

  /*-----------------------*/                                    /* BIND() */
  if (bind( serverSocket, (struct sockaddr *)&serverSockAddr, 
            sizeof( serverSockAddr )) != 0 )
    {

    }

  /*---------------*/                                        /* SETSOCKOPT() */
  if (bufsize > 0)
  {  
     size= bufsize;

     if (setsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF,
		    (void *) &size, sizeof (size)) < 0 ) {

    }

  setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF,  (void *) &size, sizeof (size));
  }


 /* BUGFIX */ 
 optlen =  sizeof(size);



if(getsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, &size, &optlen) < 0) {


    }
    else
      {

      }

if(getsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, &size, &optlen) < 0) {


    }
    else
      {

      }
  
  /*************
  if (nodelay)   
  {
      setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY,
             (char *) &one, sizeof (one));
  }
  ****************/
                                                             /* LISTEN() */
  if ( listen( serverSocket, MAX_SERVCLIENTS ) != 0 )
    {
  

    }

  return( serverSocket );
} /* createService */


/*
**===========================================================================
** 8.1			createInternalService()
**===========================================================================
** Description: Create a Unix domain listening socket on server side
**
** Parameters:  addr    - file to use
**               
**
** Returns:     returns created socket
**
** Globals:     
*/


int createInternalService(char *addr)
{
//  int len;
  int rcv_size = 1; int val;
 socklen_t len = 0;
  struct sockaddr_un serverSockAddr;
  int serverSocket;

  //  bzero( &serverSockAddr, sizeof( serverSockAddr ));
  memset( &serverSockAddr,0, sizeof(struct sockaddr_un));

  serverSockAddr.sun_family      = AF_UNIX;
  strncpy(serverSockAddr.sun_path, addr, sizeof(serverSockAddr.sun_path));


  unlink(serverSockAddr.sun_path);
  len = strlen(serverSockAddr.sun_path) + sizeof(serverSockAddr.sun_family);

  /*-----------------------*/                                    /* SOCKET() */
  if ( (serverSocket = socket(AF_UNIX, SOCK_STREAM, 0 )) == -1 )
    {

    }

  /*-----------------------*/                                    /* BIND() */
  if (bind(serverSocket, (struct sockaddr *)&serverSockAddr, sizeof(struct sockaddr_un)) != 0 )
    {

    }


 

  setsockopt(serverSocket, SOL_SOCKET, SO_RCVLOWAT,
               (char *) &rcv_size, sizeof (rcv_size));



   getsockopt(serverSocket, SOL_SOCKET, SO_RCVLOWAT,  &val, &len);
 




                                                             /* LISTEN() */
    if ( listen( serverSocket, SOMAXCONN ) != 0 )
    {

    }

  return( serverSocket );
} /* createService */



/*
**===========================================================================
** 8.0			set_timeout()
**===========================================================================
** Description: Set timeout for a receiving socket
**              
**
** Parameters:  socket
** 
** Returns:     
**
** Globals:     
*/
void set_timeout(int sock)
{
  struct timeval tv;

  tv.tv_sec = SOCKET_TIMEOUT;
  tv.tv_usec = 0;


  /* Setup recv() timeout */
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));


 /* Setup send() timeout */
  setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));


{

  int  size;
  socklen_t optlen = 0;

 if(getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, &optlen) < 0) {


    }
    else
      {

      }

if(getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, &optlen) < 0) {


    }
    else
      {

      }

}

}


void set_send_timeout(int sock)
{
  struct timeval tv;

  tv.tv_sec = 3;
  tv.tv_usec = 0;


  /* Setup receive timeout */
  setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));


}


/*
**===========================================================================
** 8.0			set_non_blocking()
**===========================================================================
** Description: Set socket as non blocking. Should be moved to 
**              communication.h   
**
** Parameters:  clientSocket
** 
** Returns:     int - non-zero if failure
**
** Globals:     
*/
void set_non_blocking(int sock)
{
	int opts;

	opts = fcntl(sock,F_GETFL);
	if (opts < 0) {

	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(sock,F_SETFL,opts) < 0) {

	}
	return;
}






/****************************************************************************/
/**                                                                        **/
/**                     LOCAL FUNCTIONS                                    **/
/**                                                                        **/
/****************************************************************************/

