#!/usr/bin/perl -w

use lib qw(/opt/datachannel/bulkadaptor/locallib);
use SendMail;

#===============================================================================
#
#                                    M A I N
#
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
# Return value :
#===============================================================================
{
  SendMail::send_mail( "Probe status", "nisse" );
}
