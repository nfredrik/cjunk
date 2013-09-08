#!/usr/bin/perl
use Tk;

$window = MainWindow->new;

($saying = $window->Label(-text => "Hello Perl/Tk world"))->pack;
($report = $window->Label(-text => "Answer"))->pack;
($action = $window->Button(-text => "Stock level", -command => \&saycount))->pack;
($action = $window->Button(-text => "Order done", -command => sub {exit;}))->pack;

MainLoop;

sub saycount {
    if ($sl) {
       $report->configure(-text => "Out O' Stock");
        $sl = 0;
    } else {
       $report->configure(-text => "Plenty in stock");
        $sl = 1;
    }
}
