#!/usr/local/bin/perl 

use lib qw(/opt/datachannel/bulkadaptor/locallib);
use TranslateProbe qw(%TeliaSonera @Num_2_String %Digit_2_String); 
use SendMail;
use constant MAX_ERROR_CODE => 10;
#use constant MAILRECEIPENT  => 'fredrik.svard@telecanetworks.se';
use DBM::Deep;

my ($CONFIG_HOME) = File::Spec->catdir( '/opt', 'datachannel', 'bulkadaptor', 'config' );
my ($RESULTFILE) = File::Spec->catfile( $CONFIG_HOME, "badprobes" );

#-------------------------------------------------------------------------------
# get_probes_result()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Get all results from all bad tests from the probes
#
# Input        : -
# Return       : None
#-------------------------------------------------------------------------------
sub get_probes_result() {

    my $db = DBM::Deep->new($RESULTFILE);

    foreach my $key ( keys %$db ) {

        # Get path
        ( $source, $dest ) = split( /\ /, $key );
        $result_string .= "<br><b>Path:";
        if ( defined( $TeliaSonera{$source} ) ) {
            $result_string .= $TeliaSonera{$source} . " ";
        }
        else {
            $result_string .= $source . " ";
        }
        if ( defined( $TeliaSonera{$dest} ) ) {
            $result_string .= $TeliaSonera{$dest} . " ";
        }
        else {
            $result_string .= $dest . " ";
        }
        $result_string .= "</b><br>";

        # Get errors for this path
        for ( $i = 1 ; $i < MAX_ERROR_CODE ; $i++ ) {
            if ( defined( $db->{$key}->[$i] ) ) {
                $result_string .= $Num_2_String[$i] .": $db->{$key}->[$i]<br>";
            }
        }
    }
    $db->clear();
    return $result_string;
}

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
    my $string = get_probes_result();

    if ( defined($string) ) {
        SendMail::send_trap( "Probe status", "$string" );
    }
    else {
    print "failed to send mail\n";
    }
    
}
