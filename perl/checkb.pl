#!/usr/bin/perl -w
use Tk;


sub pingar() {

print  "hej\n"; 
#`ping -c 3 ftp.sunet.se`;
$exit = system("ping -c 1 ftp.sunet.se");
print "$exit";
}


$cb_value = 0;
my $mw = MainWindow->new;
$cb = $mw->Checkbutton(-text => "Checkbutton" ,
                       -variable => \$cb_value,
#                       -onvalue => 15,
                       -background => 'blue',
                       -selectcolor =>'white',
                       -command => sub { print "Clicked! $cb_value\n" }
                       )->pack(-side => 'top');

$mw->Button(-text => "CB on", -background => 'green', 
            -command => sub { $cb_value = 1 })->pack(-side => 'left');
$mw->Button(-text => "CB off", -background => 'red',
            -command => sub { $cb_value = 0 })->pack(-side => 'left');

$mw->repeat(5000 => \&run);
MainLoop;

sub run {
    &pingar;
    $mw->update;
}

MainLoop;
