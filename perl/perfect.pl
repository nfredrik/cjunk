#!/usr/bin/perl -w
use strict;

$Tk::ExecuteCommand::VERSION = '1.1';

package Tk::ExecuteCommand;

use IO::Handle;
use Proc::Killfam;
#use Tk::widgets qw/LabEntry ROText/;
use Tk::widgets qw/ROText/;
use base qw/Tk::Frame/;
use strict;

Construct Tk::Widget 'ExecuteCommand';

sub Populate {

    my($self, $args) = @_;

    $self->SUPER::Populate($args);

    my $f1 = $self->Frame->pack;
    $f1->LabEntry(
        -label => 'Command to Execute',
        -labelPack => [qw/-side left/],
        -textvariable => \$self->{-command},
    )->pack(qw/-side left/);

    my $doit = $f1->Button(-text => 'Do It!')->pack(qw/-side left/);
    $self->Advertise('doit' => $doit);
    $self->_reset_doit_button;

    $self->Frame->pack(qw/pady 10/);
    $self->Label(-text => 'Command\'s stdout and stderr')->pack;

    my $text = $self->Scrolled('ROText', -wrap => 'none');
    $text->pack(qw/-expand 1 -fill both/); 
    $self->Advertise('text' => $text);
    $self->OnDestroy([$self => 'kill_command']);

    $self->{-finish} = 0;

    $self->ConfigSpecs(
        -command => [qw/METHOD command Command/, 'sleep 5; pwd'],
    );

} # end Populate

sub command {

    my($self, $command) = @_;
    $self->{-command} = $command;

} # end command

sub _flash_doit {

    # Flash "Do It" by alternating its background color.

    my($self, $option, $val1, $val2, $interval) = @_;

    if ($self->{-finish} == 0) {
        $self->Subwidget('doit')->configure($option => $val1);
        $self->idletasks;
        $self->after($interval, [\&_flash_doit, $self, $option, $val2,
                $val1, $interval]);
    }

} # end _flash_doit

sub _read_stdout {

    # Called when input is available for the output window.  Also checks
    # to see if the user has clicked Cancel.

    my($self) = @_;

    if ($self->{-finish}) {
        $self->kill_command;
    } else {
        my $h = $self->{-handle};
        if ( sysread $h, $_, 4096 ) {
            my $t = $self->Subwidget('text');
            $t->insert('end', $_);
            $t->yview('end');
        } else {
            $self->{-finish} = 1;
        }

    }
	
} # end _read_stdout

sub _reset_doit_button {

    # Establish normal "Do It" button parameters.

    my($self) = @_;

    my $doit = $self->Subwidget('doit');
    my $doit_bg = ($doit->configure(-background))[3];
    $doit->configure(
        -text       => 'Do It',
        -relief     => 'raised',
        -background => $doit_bg,
        -state      => 'normal',
        -command    => [sub {
	    my($self) = @_;
            $self->{-finish} = 0;
            $self->Subwidget('doit')->configure(
                -text   => 'Working ...',
                -relief => 'sunken',
                -state  => 'disabled'
            );
            $self->execute_command;
        }, $self],
    );

} # end _reset_doit_button

# Public methods.

sub execute_command {

    # Execute the command and capture stdout/stderr.

    my($self) = @_;
    
    my $h = IO::Handle->new;
    die "IO::Handle->new failed." unless defined $h;
    $self->{-handle} = $h;

    $self->{-pid} = open $h, $self->{-command} . ' 2>&1 |';
    if (not defined $self->{-pid}) {
        $self->Subwidget('text')->insert('end',
                "'" . $self->{-command} . "' : $!\n");
        $self->kill_command;
    }
    $h->autoflush(1);
    $self->fileevent($h, 'readable' => [\&_read_stdout, $self]);

    my $doit = $self->Subwidget('doit');
    $doit->configure(
        -text    => 'Cancel',
        -relief  => 'raised',
        -state   => 'normal',
        -command => [\&kill_command, $self],
    );

    my $doit_bg = ($doit->configure(-background))[3];
    $self->_flash_doit(-background => $doit_bg, qw/cyan 500/);
    
} # end execute_command

sub kill_command {
    
    # A click on the blinking Cancel button resumes normal operations.

    my($self) = @_;

    $self->{-finish} = 1;
    my $h = $self->{-handle};
    return unless defined $h;
    $self->fileevent($h, 'readable' => ''); # clear handler
    killfam 'TERM', $self->{-pid} if defined $self->{-pid};
    close $h;
    $self->_reset_doit_button;

} # end kill_command

1;

__END__

=head1 NAME

Tk::ExecuteCommand - execute a command asynchronously (non-blocking).

=for pm Tk/ExecuteCommand.pm

=for category Widgets

=head1 SYNOPSIS

S<    >I<$exec> = I<$parent>-E<gt>B<ExecuteCommand>;

=head1 DESCRIPTION

Tk::ExecuteCommand runs a command yet still allows Tk events to flow.  All
command output and errors are displayed in a window.

This ExecuteCommand mega widget is composed of an LabEntry widget for
command entry, a "Do It" Button that initiates command execution, and
a ROText widget that collects command execution output.

While the command is executing, the "Do It" Button changes to a "Cancel"
Button that can prematurely kill the executing command. The B<kill_command>
method does the same thing programmatically.

=over 4

=item B<-command>

The command to execute asynchronously.

=back

=head1 METHODS

=over 4

=item C<$exec-E<gt>B<execute_command>;>

Initiates command execution.

=item C<$exec-E<gt>B<kill_command>;>

Terminates the command.  This subroutine is called automatically via an
OnDestroy handler when the ExecuteCommand widget goes away.

=back

=head1 EXAMPLE

I<$exec> = I<$mw>-E<gt>B<ExecuteCommand>;

=head1 KEYWORDS

exec, command, fork, asynchronous, non-blocking, widget
=head1 COPYRIGHT

Copyright (C) 1999 - 2001 Stephen O. Lidie. All rights reserved.

This program is free software; you can redistribute it and/or modify it under
the same terms as Perl itself.

=cut
