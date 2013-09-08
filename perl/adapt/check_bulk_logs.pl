#!/usr/local/bin/perl -w 

use lib qw(/opt/datachannel/bulkadaptor/locallib);
use SendMail;
use File::Basename;
use File::Spec;
use strict;

my ($LOG_HOME) = File::Spec->catdir( '/opt', 'datachannel', 'log' );
my ($CONFIG_HOME) = File::Spec->catdir( '/opt', 'datachannel', 'bulkadaptor');
my ($FLAGFILE) = File::Spec->catfile( $CONFIG_HOME, "flag.txt" );

#-------------------------------------------------------------------------------
# check_if_any_log_files( )
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
# Input        : -
# Return       :
#-------------------------------------------------------------------------------

sub check_if_any_log_files() {
    my $i;
    my @files = ();

    system("ls $LOG_HOME/*.log > /dev/null 2>&1");
    if ( $? != 0 ) {
        return 0;
    }
    return 1;
}

#-------------------------------------------------------------------------------
# check_for_errors( )
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
# Input        : -
# Return       :
#-------------------------------------------------------------------------------

sub check_for_errors() {
    my $i;
    my $string ="";

    for $i (`ls $LOG_HOME/\*.log`) {

        chomp($i);
        open( FILE, "<$i" );

        while (<FILE>) {
            if ( $_ =~ /\tF\t(.*)/ ) {
                $string .="Failure: $1<br>";
            }
            if ( $_ =~ /\tW\t(.*)/ ) {
                $string .="Warning: $1<br>";
            }
        }
        close(FILE);
    }
    return $string;
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

if ( !( -e "$FLAGFILE" ) ) {

    if ( check_if_any_log_files() ) {

        my $result = check_for_errors();

        if ( $result ne "" ) {
            SendMail::send_trap( "Proviso Bulk error", "$result" );
            system("touch $FLAGFILE");
        }
    }
}
