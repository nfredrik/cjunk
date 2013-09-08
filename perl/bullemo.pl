#!/usr/bin/perl -w
use Tk;


sub pingar() {

print  "hej\n"; 
#`ping -c 3 ftp.sunet.se`;
$exit = system("ping -c 1 ftp.sunet.sf");
print "$exit";
}

my $mw = MainWindow->new;
$mw->title("Good Window");
$mw->Label(-text => "This window looks much more organized, and less haphazard\n" .
    "because we used some options to make it look nice")->pack;

# -selectcolor => '#45',
$mw->Button(   -text => 'Ping sunet', -command => [\&pingar] )->pack;
$mw->Button(-text => 'Stop High Voltage Supply', -command => sub { print "hej\n";exit })->pack;
$mw->Button(-text => 'Init HW', -command => sub { exit })->pack;
$mw->Button(-text => 'rem single', -command => sub { exit })->pack;


MainLoop; 




__END__

$mw->Button(-text => "Exit",
            -command => sub { exit })->pack(-side => 'bottom', 
                                            -expand => 1,
                                            -fill => 'x');
$mw->Checkbutton(-text => "I like it!")->pack(-side => 'left',
                                              -expand => 1);
$mw->Checkbutton(-text => "I hate it!")->pack(-side => 'left', 
                                              -expand => 1);
$mw->Checkbutton(-text => "I don't care")->pack(-side => 'left', 
                                                -expand => 1);

