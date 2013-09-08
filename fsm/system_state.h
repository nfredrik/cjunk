
#ifndef _SYSTEM_STATE_INCLUDED
#define _SYSTEM_STATE_INCLUDED
//
// C++ Interface: system_state
//
// Description: 
//
//
// Author: Fredrik Svärd <fredrik@c-rad.se>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
typedef enum _CONDITION {
  CS_SOFTWARE_INITIALIZED,           /* The application has been initialized correctly */
  SET_CONFIGURATION,                 /* External software issues a set configuration */
  SET_STANDBY,                       /* External software issues a set standby */
  CS_SOFTWARE_TIMEOUT,               /* CS software sets timeout after waiting for hardware too long */
  HARDWARE_READY,                    /* Hardware ready to go operational*/
  CS_SOFTWARE_FAILURE,               /* Fatal failure for CS software */
  CS_HARDWARE_FAILURE                /* Fatal failure for CS hardware */
                                     
} CONDITION;


SYSTEM_STATE get_system_state();

int change_system_state(CONDITION c, int ttyfd);

void set_standby_system_state();


typedef enum _COND {

  COMMENCE_OFFSET_COLLECTION,
  TERMINATE_OFFSET_COLLECTION
} COND;


BACKGROUND_STATE get_offset_mode();

int change_background_mode(COND c);

#endif

