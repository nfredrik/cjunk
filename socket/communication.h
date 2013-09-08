/*
	                     *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : Gemini                                                      **
** filename  : communication.h                                             **
** version   : 2                                                           **
** date      : September 18, 2007                                          **
**                                                                         **
*****************************************************************************
**                                                                         **
** Copyright (c) 2007,  C-RAD Imaging                                      **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************

*/
#ifndef _COMMUNICATION_INCLUDED
#define _COMMUNICATION_INCLUDED

/*
*****************************************************************************
** 1  DESCRIPTION.
*****************************************************************************
**
** This file defines functions to setup a server or client session,
** both for TCP session and Unix domain sockets
** 

TODO:
- Get the setsockopt() to work!!
- Replace functions like error(), errorc(), with log4c logging


*/

/*
*****************************************************************************
** 2  HISTORY OF DEVELOPMENT.
*****************************************************************************
** date       responsible              notes
** --------   --------------------     --------------------------------------
** 2007-10-30 Fredrik Svärd            Initial version
*/

/*
*****************************************************************************
** 3  DEFINITIONS.
*****************************************************************************
*/


/*
**===========================================================================
** 3.1  Manifest constants.
**===========================================================================
*/

#define SERVER_PORT      (4390)
#define HW_SERVER   "hw_server"
#define CMD_HANDLER "cmd_handler"
#define FRAME_PROCESSER "frame_processer"
#define FRAME_GRABBER "frame_grabber"
/*
**===========================================================================
** 3.2  Type definitions.
**===========================================================================
*/

/*
**===========================================================================
** 3.3  Macros.
**===========================================================================
*/

/*
*****************************************************************************
** 4  EXTERNAL REFERENCES.
*****************************************************************************
*/

/*
**===========================================================================
** 4.1  Functions.
**===========================================================================
*/


int  createConnection(int port, char *addr, int bufsize);

int createUDPConnection(int port, char *addr, int bufsize);

int  create_service(unsigned short port, int bufsize, int nodelay);

int  createInternalConnection(char *addr);

int  createInternalService(char *addr);

void set_timeout(int sock);

void set_non_blocking(int sock);

void ping_peer(int sock);

void stop_acq(int sock);


/*
**===========================================================================
** 4.2  Variables.
**===========================================================================
*/
#ifndef _COMMUNICATION_C_SRC

// FIXME Clean here
char   slaskbuff[1000];  /*kalle*/
char   PROGRAM  [200];
extern long start_time;
extern long end_time;
extern long connectTimeS, connectTimeU;
extern long rtdTimeS, rtdTimeU;
extern int  done;
#endif /*_COMMUNICATION_C_SRC */

#endif /* _COMMUNICATION_INCLUDED */

