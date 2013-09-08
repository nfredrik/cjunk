#ifndef _TAG_INCLUDED
#define _TAG_INCLUDED
//
// C++ Interface: tag
//
// Description: 
//
//
// Author: Fredrik Svärd <fredrik@c-rad.se>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cs_commands.h"



int tie_tag_2_socket(long cmd_tag, int socket);

void untie_tag_2_socket(u32 cmd_tag);

void delete_tags_assocs_to_socket(int socket);

int get_socket_in_hash(u32 cmd_tag);

//void check_multi_cmd_tag(latest_reply_t *late);

void delete_multi_cmd_tag(u32 cmd_tag, time_t old_time);

void purge_hash();

void print_ptr_2_hash();

#endif
