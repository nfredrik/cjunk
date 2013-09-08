/*
*  C Implementation: system_state
*
* Description: 
*
*
* Author: Fredrik Svärd <fredrik@c-rad.se>, (C) 2008
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include "log.h"
#include "cs_commands.h"
#include "system_state.h"
#include "hardware.h"

//extern int ttyfd;

/* Holds the system state */
static SYSTEM_STATE system_state = INIT;

/* Holds the background state */
static BACKGROUND_STATE background_state = NO_COLLECTING;

#define FAILING_RC_RETURNED   0

SYSTEM_STATE get_system_state() {

return system_state;
}



/*
**===========================================================================
** 8.0			result_system_state()
**===========================================================================
** Description:  
**               
**              
**              
** Parameters:  
**              
**
** Returns:     
**
** Globals:     
 
*/
static char *result_system_state(SYSTEM_STATE s) {
  switch(s) {
  case INIT:         return "INIT";
  case STANDBY:      return "STANDBY";
  case WARMUP:       return "WARMUP";
  case OPERATIONAL : return "OPERATIONAL";
  case CHILL :       return "CHILL";
  case FAILED:       return "FAILED";
  case REDUCED:      return "REDUCED";
  default:
    return "Unknown system state";
  }
}

static char *conditions(CONDITION cnd) {
  switch(cnd) {

  case CS_SOFTWARE_INITIALIZED:         return "CS_SOFTWARE_INITIALIZED";
  case SET_CONFIGURATION:              return "SET_CONFIGURATION";
  case SET_STANDBY:                    return "SET_STANDBY";
  case CS_SOFTWARE_TIMEOUT:            return "CS_SOFTWARE_TIMEOUT";
  case HARDWARE_READY:                 return "HARDWARE_READY";
  case CS_SOFTWARE_FAILURE:            return "CS_SOFTWARE_FAILURE";
  case CS_HARDWARE_FAILURE:             return "CS_HARDWARE_FAILURE";
  default:
    return "Unknown condition";
  }
}




int change_system_state(CONDITION c, int ttyfd) {


LOG2FILE(LOG4C_PRIORITY_NOTICE,"current:system state:%s, condition:%s",result_system_state(system_state), conditions(c));

   switch(system_state) {

/****************************************************************************/
/**                                                                        **/
/**                               INIT                                     **/
/**                                                                        **/
/****************************************************************************/
   case INIT:

      switch(c) {
      case CS_SOFTWARE_INITIALIZED:
         system_state = STANDBY; 
         break;

      case CS_SOFTWARE_FAILURE:
         system_state = REDUCED; 
         break;

    case CS_HARDWARE_FAILURE:
         system_state = FAILED; 
         break;

    default:
    LOG2FILE(LOG4C_PRIORITY_ERROR,"wrong current state or wrong condition!");
#if FAILING_RC_RETURNED
    return -1;
#endif
      break;
   }

     break;

/****************************************************************************/
/**                                                                        **/
/**                               STANDBY                                  **/
/**                                                                        **/
/****************************************************************************/

  case STANDBY:
     switch(c) {
      case SET_CONFIGURATION:
         system_state = WARMUP; 
         break;

      case CS_SOFTWARE_FAILURE:
         system_state = REDUCED; 
         break;

    case CS_HARDWARE_FAILURE:
         system_state = FAILED; 
         break;

    default:
    LOG2FILE(LOG4C_PRIORITY_ERROR,"wrong current state or wrong condition!");
 #if FAILING_RC_RETURNED
    return -1;
#endif
      break;
   }

     break;

/****************************************************************************/
/**                                                                        **/
/**                               WARMUP                                   **/
/**                                                                        **/
/****************************************************************************/

  case WARMUP:
   switch(c) {
 
     case SET_STANDBY:
         system_state = CHILL; 
         break;

     case HARDWARE_READY:
          system_state = OPERATIONAL; 
         break; 

      case CS_SOFTWARE_FAILURE:
         system_state = REDUCED; 
         break;

    case CS_SOFTWARE_TIMEOUT:
    case CS_HARDWARE_FAILURE:
         system_state = FAILED; 
         break;

    default:
    LOG2FILE(LOG4C_PRIORITY_ERROR,"wrong current state or wrong condition!");
#if FAILING_RC_RETURNED
    return -1;
#endif
      break;
   }
     break;

/****************************************************************************/
/**                                                                        **/
/**                               OPERATIONAL                              **/
/**                                                                        **/
/****************************************************************************/

  case OPERATIONAL:

  switch(c) {

  case SET_CONFIGURATION:
         system_state = WARMUP; 
         break;
 
     case SET_STANDBY:
         system_state = CHILL; 
         break;

      case CS_SOFTWARE_FAILURE:
         system_state = REDUCED; 
         break;

    case CS_HARDWARE_FAILURE:
         system_state = FAILED; 
         break;

    default:
    LOG2FILE(LOG4C_PRIORITY_ERROR,"wrong current state or wrong condition!");
#if FAILING_RC_RETURNED
    return -1;
#endif
      break;
   }

     break;

/****************************************************************************/
/**                                                                        **/
/**                               CHILL                                    **/
/**                                                                        **/
/****************************************************************************/

  case CHILL:
 switch(c) {

     case HARDWARE_READY:
          system_state = STANDBY; 
         break; 

      case CS_SOFTWARE_FAILURE:
         system_state = REDUCED; 
         break;

    case CS_HARDWARE_FAILURE:
         system_state = FAILED; 
         break;

    default:
    LOG2FILE(LOG4C_PRIORITY_ERROR,"wrong current state or wrong condition!");
#if FAILING_RC_RETURNED
    return -1;
#endif
      break;
   }

     break;

/****************************************************************************/
/**                                                                        **/
/**                               FAILED                                   **/
/**                                                                        **/
/****************************************************************************/
  case FAILED:

switch(c) {

    default:
    LOG2FILE(LOG4C_PRIORITY_ERROR,"wrong current state or wrong condition!");
#if FAILING_RC_RETURNED
    return -1;
#endif
      break;
   }

     break;

/****************************************************************************/
/**                                                                        **/
/**                               REDUCED                                  **/
/**                                                                        **/
/****************************************************************************/

  case REDUCED:

switch(c) {

    default:
    LOG2FILE(LOG4C_PRIORITY_ERROR,"wrong current state or wrong condition!");
#if FAILING_RC_RETURNED
    return -1;
#endif
      break;
   }
     break;


/****************************************************************************/
/**                                                                        **/
/**                               DEFAULT                                  **/
/**                                                                        **/
/****************************************************************************/
   default:
   LOG2FILE(LOG4C_PRIORITY_ERROR,"wrong current state!!!"); 
#if FAILING_RC_RETURNED
    return -1;
#endif
      break;

   }

LOG2FILE(LOG4C_PRIORITY_NOTICE,"new state:system state:%s",result_system_state(system_state));
print_state(ttyfd,system_state);
 return 0;
}

void set_standby_system_state() {

 system_state = STANDBY;
}


int change_background_mode(COND c) {

  switch(background_state) {

/****************************************************************************/
/**                                                                        **/
/**                               OFFSET_COLLECTING                        **/
/**                                                                        **/
/****************************************************************************/
   case OFFSET_COLLECTING:

      switch(c) {
      case TERMINATE_OFFSET_COLLECTION:
         background_state = NO_COLLECTING; 
         break;

    default:
    LOG2FILE(LOG4C_PRIORITY_ERROR,"wrong current state or wrong condition!");
#if FAILING_RC_RETURNED
    return -1;
#endif
      break;
   }

     break;

/****************************************************************************/
/**                                                                        **/
/**                               NO_COLLECTING                            **/
/**                                                                        **/
/****************************************************************************/
   case NO_COLLECTING:

      switch(c) {
      case COMMENCE_OFFSET_COLLECTION:
         background_state = OFFSET_COLLECTING; 
         break;
    default:
    LOG2FILE(LOG4C_PRIORITY_ERROR,"wrong current state or wrong condition!");
#if FAILING_RC_RETURNED
    return -1;
#endif
      break;
   }

     break;

/****************************************************************************/
/**                                                                        **/
/**                               DEFAULT                                  **/
/**                                                                        **/
/****************************************************************************/
   default:
   LOG2FILE(LOG4C_PRIORITY_ERROR,"wrong current state!!!"); 
#if FAILING_RC_RETURNED
    return -1;
#endif
      break;

   }

  return 0;
  }


BACKGROUND_STATE get_offset_mode() {

return background_state;
}



