#!/usr/bin/perl -w
use Tk;
use Net::Ping;

@alla = ('192.168.0.1','192.168.2.1','192.168.2.1','192.168.2.1');
sub pingar() {

@hi = shift;

$p = Net::Ping->new("tcp");
    foreach $host (@hi)
    {
        print "$host is ";
        print "NOT " unless $p->ping($host,1);
        print "reachable.\n";
        sleep(1);
    }
    $p->close();
}

print "$alla[0]";

my $mw = MainWindow->new;
$mw->title("Good Window");

$mw->Label(-text => $alla[0] )->pack;
$mw->Label(-text => $alla[1] )->pack;
$mw->Label(-text => $alla[2] )->pack;
$mw->Label(-text => $alla[3] )->pack;

$mw->Button(-text => "Exit",
            -command => sub { exit })->pack(-side => 'bottom', 
                                            -expand => 1,
                                            -fill => 'x');


#$mw->repeat(1000 => \&pingar(@alla) );
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

