#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
#                     MAIL SUB ROUTINE
#
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# FileName      : SendMail.pm
# Author        : Fredrik Svärd
# Date          : 
# Version       : 1.0
# Subjet        :
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package SendMail;

use Mail::SendEasy;
use POSIX;
use strict;
use warnings;
require Exporter;
#use base 'Exporter';
our (@ISA) =("Exporter");
our ($VERSION)="1.0.1";

use constant MAIL_RECIPIENTS => 'fredrik.svard@c-rad.se'


#-------------------------------------------------------------------------------
# send_mail()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Send mail to receipients with a relevant slogan and text message
#
# Input        : -
# Return       : In case of failure, error code.
#-------------------------------------------------------------------------------
sub send_mail($$) {
    my $heading = shift();
    my $text    = shift();

    my $mail = new Mail::SendEasy();

    my $status = $mail->send(
        from       => 'nimrod@telianetworks.net',
        from_title => 'Proviso DataLoad Bulk',

        #reply   => 'noreply@telianetworks.net' ,  doesn't work
        #error   => 'noreply@telianetworks.net' ,  doesnt' work
        to => MAIL_RECIPIENTS,
        # cc      => 'recpcopy@domain.foo' ,
        subject => $heading,
        msg     => "hejhopp!",   # plain ASCII
        html    => $text,          # HTML  
        msgid   => "0101",         # Msg-ID:0101 in mail header!
	# anex  => filename,
	# zipanex => filename.zip
    );
    
    return 0;

#    if ( !$status ) {
#        return $mail->error;
#    }
}

