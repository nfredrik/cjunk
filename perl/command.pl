#!/usr/bin/perl -w
use strict;
use Tk;
use Tk::ExecuteCommand;

 my $mw = MainWindow->new;

my  $ec = $mw->ExecuteCommand(
     -command    => '',
     -entryWidth => 50,
     -height     => 10,
     -label      => '',
     -text       => 'Execute',
 )->pack;
 $ec->configure(-command => 'top');
 $ec->execute_command;
 $ec->bell;
 $ec->update;
