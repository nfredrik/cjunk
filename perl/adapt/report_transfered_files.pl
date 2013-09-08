#!/usr/local/bin/perl -w 

use lib qw(/opt/datachannel/bulkadaptor/locallib);
use SendMail;
use File::Basename;
use File::Spec;
use strict;

my ($CONFIG_HOME) =
  File::Spec->catdir( '/opt', 'datachannel', 'bulkadaptor','config' );
my ($COUNTERFILE) = File::Spec->catfile( $CONFIG_HOME, "counters" );
my %Remember_This;

#-------------------------------------------------------------------------------
# read_clear_counter
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Reads file counter
#
# Input        : -
# Return       : 0 - No, 1 - Yes
#-------------------------------------------------------------------------------
sub read_clear_counter() {
    my $arg = shift();
    my $return_value;

    unless ( dbmopen( %Remember_This, $COUNTERFILE, 0777 ) ) {
    }

    $return_value = $Remember_This{"filecounter"} . " files has been transfered last 24 hours<br>";
    $return_value .= $Remember_This{"rloginfail"} . " failed logins <br>";
    $return_value .= $Remember_This{"rvalidfail"} . " failed validations <br>";
    $return_value .= $Remember_This{"rfilefail"}  . " failed file conversions <br>";
    $return_value .= $Remember_This{"remptyfile"} . " empty files <br>\n";
    
    $Remember_This{"filecounter"} = 0;
    $Remember_This{"rloginfail"} = 0;
    $Remember_This{"rvalidfail"} = 0;
    $Remember_This{"rfilefail"} = 0;
    $Remember_This{"remptyfile"} = 0;
    return $return_value;

    unless ( dbmclose(%Remember_This) ) {
    }
    return 0;
}

#===============================================================================
#
#                                    M A I N
#
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
# Return value :
#   0 - Succeeded
#   1 - Failed
#   2 - Warning
#===============================================================================

my $r = read_clear_counter();
#print "$r files has been transfered this week!\n";
 if(SendMail::send_trap( "Proviso Bulk report", "$r" ))
 {
 }
exit 0;
