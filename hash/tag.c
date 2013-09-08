/*
*  C Implementation: tag
*
* Description: 
*
*
* Author: Fredrik Svärd <fredrik@c-rad.se>, (C) 2008
*
* Copyright: See COPYING file that comes with this distribution
*
*/
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include "uthash.h"
#include "log.h"
#include "tag.h"
#include "cs_commands.h"






#define  TIMEOUT_CMD_TAG  10

struct assoc_2_socket {
  long cmd_tag;                    /* key */
  int socket;                      /* value */   
  time_t time;                     /* remember creation of key/value */
  UT_hash_handle hh;               /* makes this structure hashable */
};

static struct assoc_2_socket *ptr_2_hash = NULL;


#undef uthash_fatal
#define uthash_fatal(msg) t_my_fatal_function(msg)

void t_my_fatal_function(char *msg) {
 LOG2FILE(LOG4C_PRIORITY_ERROR,"%s",msg);
}


/*
**===========================================================================
** 8.0			delete_tie_tag_2_socket()
**===========================================================================
** Description: Delete tie. Verify that the key->value exists before calling
**              
**
** Parameters:  cmd_tag (key)
** Returns:     int - non-zero if failure
**
** Globals:     ptr_2_hash
*/
static void delete_tie_tag_2_socket(struct assoc_2_socket *user) {
    HASH_DEL( ptr_2_hash, user);  /* user: pointer to delete */
}


/*
**===========================================================================
** 8.0			find_socket_in_hash()
**===========================================================================
** Description: Get the socket id with the help of the cmd_tag as key
**              
**
** Parameters:  cmd_tag (key)
**              
** Returns:     socketid if success, NULL on failure
**
** Globals:     ptr_2_hash
*/
static struct assoc_2_socket *find_socket_in_hash(long cmd_tag) {
    struct assoc_2_socket *s;

    HASH_FIND_INT( ptr_2_hash, &cmd_tag, s );  /* s: output pointer */
    return s;
}



/*
**===========================================================================
** 8.0			tie_tag_2_socket()
**===========================================================================
** Description: Tie a a command tag to a socket to be able to reply on
**              correct socket later on.
**              Failure on malloc will call my_fatal_function
**
** Parameters:  cmd_tag  (key)
**              socket (value)
** Returns:     Zero
**
** Globals:     ptr_2_hash
*/
int tie_tag_2_socket(long cmd_tag, int socket) {
    struct assoc_2_socket *s;

    s = malloc(sizeof(struct assoc_2_socket));
    if (s == NULL) {
       LOG2FILE(LOG4C_PRIORITY_ERROR, "malloc failed");
    }  
    s->cmd_tag = cmd_tag;
    s->socket = socket;
    s->time = time(NULL);
    HASH_ADD_INT( ptr_2_hash, cmd_tag, s );  /* cmd_tag: name of key field */
    return 0;
}


/*
**===========================================================================
** 8.0			delete_tags_assocs_to_socket()
**===========================================================================
** Description: Delete everthing associated with specified socket
**              
**
** Parameters:  cmd_tag (key)
** Returns:     int - non-zero if failure
**
** Globals:     ptr_2_hash
*/
void delete_tags_assocs_to_socket(int socket) {

   struct assoc_2_socket *s;

   for(s=ptr_2_hash; s != NULL; s=s->hh.next) {

      if(socket == s->socket) {
      LOG2FILE(LOG4C_PRIORITY_INFO,"cmd_tag %lx: socket %d", s->cmd_tag, s->socket);
      delete_tie_tag_2_socket(s);
      }
    }
}


/*
**===========================================================================
** 8.0			get_socket_in_hash()
**===========================================================================
** Description: 
**              
**
** Parameters:  cmd_tag (key)
** Returns:     int - non-zero if failure
**
** Globals:     ptr_2_hash
*/

int get_socket_in_hash(u32 cmd_tag) {
    struct assoc_2_socket *s;

    HASH_FIND_INT( ptr_2_hash, &cmd_tag, s );  /* s: output pointer */

    if(s == NULL) return 0;

    return (s->socket);
}



/*
**===========================================================================
** 8.0			untie_tag_2_socket()
**===========================================================================
** Description: 
**              
**
** Parameters:  cmd_tag (key)
** Returns:     int - non-zero if failure
**
** Globals:     ptr_2_hash
*/


void untie_tag_2_socket(u32 cmd_tag) {

    struct assoc_2_socket *s; 

    s = find_socket_in_hash(cmd_tag); 
    HASH_DEL( ptr_2_hash, s);  /* s: pointer to delete */
}

/*
**===========================================================================
** 8.0			purge_hash()
**===========================================================================
** Description: Purge hash from old (key,value)-pairs.
**              
**
** Parameters:  clientSocket
**              cmd_tag 
** Returns:     int - non-zero if failure
**
** Globals:     ptr_2_hash
*/
void purge_hash() {
    struct assoc_2_socket *s;
    time_t now;

    time(&now);
    
    for(s=ptr_2_hash; s != NULL; s=s->hh.next) {

      if ((s->time + TIMEOUT_CMD_TAG) <= now) {

         LOG2FILE(LOG4C_PRIORITY_INFO,"Found and deleting old assoc cmd_tag: %lx, socket:%d, time: %ld",
	       s->cmd_tag, s->socket, s->time);
	  delete_tie_tag_2_socket(s);
      }
    }
}
/*
**===========================================================================
** 8.0			delete_multi_cmd_tag()
**===========================================================================
** Description: Check and delete if everything ceased.
**              
**
** Parameters:  
**              
** Returns:     
**
** Globals:     
*/

void delete_multi_cmd_tag(u32 cmd_tag, time_t old_time) {
    struct tm *local_time;
    struct assoc_2_socket *s;
    time_t now;

    now = time(NULL);
    
    if ((old_time + TIMEOUT_CMD_TAG) <= now) {
      if ((s = find_socket_in_hash(cmd_tag)) == NULL) {
         /* Don't now if this should be regarded as an error ...*/
#if ZERO
         LOG2FILE(LOG4C_PRIORITY_NOTICE,"Did not find socket in hash, cmd_tag:%lx", cmd_tag);
#endif
         /* Probably deleted when the socket was closed, don't check no more ... */
         old_time = 0;
         return; 
      }
     local_time = localtime(&(s->time));
     LOG2FILE(LOG4C_PRIORITY_INFO,"deleting cmd_tag %lx: socket %d time: %02d:%02d:%02d", 
     s->cmd_tag, s->socket, local_time->tm_hour, local_time->tm_min,local_time->tm_sec );
	  delete_tie_tag_2_socket(s);
    }
}

/*
**===========================================================================
** 8.0			print_ptr_2_hash()
**===========================================================================
** Description: Check and delete if everything ceased.
**              
**
** Parameters:  
**              
** Returns:     
**
** Globals:     
*/
void print_ptr_2_hash() {
    struct tm *local_time;
    struct assoc_2_socket *s;

    for(s=ptr_2_hash; s != NULL; s=s->hh.next) {
        local_time = localtime(&(s->time));
      LOG2FILE(LOG4C_PRIORITY_INFO,"cmd_tag %lx: socket %d time: %02d:%02d:%02d", 
      s->cmd_tag, s->socket, local_time->tm_hour, local_time->tm_min,local_time->tm_sec );
    }
}


