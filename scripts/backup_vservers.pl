#!/usr/bin/perl -w
#
# Script to backup trac and svn repository from all _running_ vserver-guests
#
use System2;
use Time::Format qw(%time %strftime %manip);
use Mail::SendEasy;
use constant FAILURE => 42;
use constant SUCCESS => 0;

sub send_mail {

    my @text = shift();
    my $mail = new Mail::SendEasy(
        smtp => 'smtp.jamtport.se',
        user => 'trac_epost@solida.se',
        pass => PLRC83kWt9ZU,
    );

    my $status = $mail->send(
        from    => 'kontest@solida.se',
        to      => 'kontest_backup975@solida.se',
        subject => 'Error in backup',
        html    => "@text",
        msgid   => "0101",                         # Msg-ID:0101 in mail header!
        port    => 26
    );

    if ( !$status ) {
        print "Error:" . $mail->error . "\n";
    }
}

sub execute_cmd {

    my @cmd = @_;
#    print "@cmd" . "\n";

    my ( $stdout, $stderr ) = system2(@cmd);
    my ( $exit_value, $signal_num, $dumped_core ) = &System2::exit_status($?);
    if ( $exit_value != 0 ) {
        print "We failed to execute: @cmd, send mail!\n";
        send_mail(@cmd);
        exit FAILURE;
    }
    return $stdout;
}

# Find running guests ...
@args = ("/usr/sbin/vserver-stat");
$out  = execute_cmd(@args);
@rows = split( "\n", $out );

# Filter out the guest name from the output
$i = 0;
for (@rows) {
    if ( $_ =~ /^[0-9]+/ ) {
        @fields = split( "\ ", $rows[$i] );
        push @guests, $fields[$#fields];
    }
    $i++;
}

$backup_src     = '/var/kont_backups/';
$trac_guest_src = $backup_src . 'trac/';
$svn_guest_src  = $backup_src . 'svn/';
$guest_mirror   = '/var/lib/vservers/';

# we gathered all guests, now loop
foreach $guest (@guests) {

    # remove old trac and svn directories
    @args = (
        '/usr/sbin/vserver', $guest, 'exec', 'rm', '-fr', $trac_guest_src,
        $svn_guest_src
    );
    execute_cmd(@args);

    # trac hotcopy
    $trac_home = '/var/trac';
    my @args = (
        '/usr/sbin/vserver', $guest, 'exec', 'trac-admin', $trac_home,
        'hotcopy', $trac_guest_src
    );
    execute_cmd(@args);

    # svn hotcopy
    $svn_home = '/var/svn/repos';
    @args     = (
        '/usr/sbin/vserver', $guest, 'exec', 'svnadmin', 'hotcopy', $svn_home,
        $svn_guest_src
    );
    execute_cmd(@args);

    # copy trac and svn-hotcopy to host 'environment'
    $dst = $backup_src . $guest;

    # create directory
    if ( !( -d $dst ) ) {
        mkdir $dst;
    }
    else {
        $dst_sub = $dst . '/*';
        @args = ( 'rm', '-fr', $dst_sub );
        execute_cmd(@args);
    }

    $src = $guest_mirror . $guest . $svn_guest_src;
    @args = ( 'cp', '-r', $src, $dst );
    execute_cmd(@args);

    $src = $guest_mirror . $guest . $trac_guest_src;
    @args = ( 'cp', '-pr', $src, $dst );
    execute_cmd(@args);
}

exit SUCCESS;

__END__
CTX   PROC    VSZ    RSS  userTIME   sysTIME    UPTIME NAME
40004   64 618.3M  74.4M   3m13s96   0m03s58   5d23h32 kap_ab
40006   58 515.8M  17.3M   0m00s23   0m00s48   1h00m13 svn_rm
$time{ 'yymmdd-hh:mm', $hour };
$time{'yymmdd-hh:mm'};
print 'Exit code:' . $exit_value . "\n"; 
print "@args" . "\n";
