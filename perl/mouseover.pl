#!/usr/bin/perl
use strict;
use Tk;

my $mw = MainWindow->new;
# Mainwindow: sizex/y, positionx/y
$mw->geometry("150x100+100+120");

# create all pictureobjects
my %picture_stock = create_picture_stock($mw);

&create_mouseover_button;
MainLoop;


sub create_mouseover_button {
	# tells you the subroutine
	$mw->Label(-text => "changing_button",
		)->pack(-side => 'left');

	# paint a button with a picture, nothing special here
	my $button = $mw->Button(
		-background => 'white',
		-image => $picture_stock{'reload'},
		-command => sub { exit; }
		# sticky=>\'w\' put all buttons left, not centered like default
		)->pack(-side => 'left');
	# create additional events for this button
	# do something when the mouse is over the button
	# Great article about binding: http://www.foo.be/docs/tpj/issues/vol2_3/tpj0203-0006.html
	$button->bind('<Enter>' => sub { &mouseover_change("enter", \$button); });
	# do something when the mouse leaves the button
	$button->bind('<Leave>' => sub { &mouseover_change("leave", \$button); });
}

sub mouseover_change {
	# this sub is called when the button is entered or left
	# two actions possible: enter/leave
	# rbutton is the button to be changed
	my ($action, $rbutton) = @_;
	# get reference to the right picture
	my $pic_obj = 
		$action eq "leave" ?  $picture_stock{reload} : $picture_stock{reload_dark};

	# tell the widget to take this picture
	${$rbutton}->configure( -image => $pic_obj);
}

sub create_picture_stock {
	my %pictures;
	
	foreach ( qw/reload reload_dark/ ) {
		no strict;
		my ($format, $pic_data) = &{"pic_" . $_};
		use strict;
		# create the photo-objects
		$pictures{$_} = $mw->Photo(
			-format => $format, 
			-data => $pic_data);
	}

	return %pictures;
}

# 
# From here on only data of imagefiles
#

sub pic_reload {
	# Tk::Photo needs a mime64-based string
	# You can create it with: mimencode input.gif >output.mime64
	my $format = "gif";
	my $pic =<<'EOPIC';
R0lGODlhHAAcAPcAAACcAAicEBCcEBilISGlMSmtMTGtQjmtUjm1Qkq1UlK1c1K9c1pajFpj
jFq9/2NjjGNjlGNrlGO9hGtrnGtznGul52vGc3NznHNzpXN7pXN7rXOEtXPGhHPGjHPGnHuE
rXuEtXuMtXuMvXvOhISUxoSczoSc1oSlzoSl1oSt3oSt54TOjIyc54yl1oyt3oy154zOvZSc
55S155S975TWvZyl55zWnKWl/6XW3qXW/62t/63G563W/7W157W177W977W9/7XevbXe77Xe
/72957299729/73e773e/8bG/8bO58bO/8bW/8bnzsbn/87O/87W3s7W587W787W/87n3s7n
/87vzs7v3tbW3tbe3tbn/9bv79bv/97e597n597v3t7v797v/+fn5+fv7+f3/+/v7+/v9+/3
//f39/f/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/////////////////////////////////ywAAAAAHAAcAAAI/gDVCBxIsKDBgwgTKjT4IgUK
hxAfSkyhIcpCNS8Gotm4sYyZMmXGaEBhUWFGhGnSnDmzQUUJJQpdoDmYcuUZDQ4qkFCSBqHM
gipXkhmqYUPRCwl/EvxiAQGCFVsA/KjBggVShC1mCkxjA4BXrwMAaDGi4waFhCjKDGzyta1Y
LVWAXD1YQq2aNAgAcKBCJcFXuFWSTEhY924TAU3IhNlC4G+VKksGIyQxJg2aEUEUh3FbxQkT
JpIPkhCjksPQMGYeP2bixAmSJxESjhaqOYzqzk6OIFkSG6EIMWeGugXQ+QgBIUOWQEgYosvp
th2qMBFi4ACS5A8SgvCiuS0BCQoEgQCAwQO79ixh0g/3qmAIDx5LGCT8gCW9FgAd/H5t/x6+
fIT0caHFfazBIIEHOPTHQw7xJZQBFgI+1hoTSFzn3ns5JPHfQRlAAZhrrg1x4YI55GDEhgZl
QMQPLPrwQw8+9CDjjDLKgGJBGWCQwQU5ZtBjjzpeoOONFxVp5JFIFhkQAAA7
EOPIC
	return $format,$pic;
}

sub pic_reload_dark {
	# Tk::Photo needs a mime64-based string
	# You can create it with: mimencode input.gif >output.mime64
	my $format = "gif";
	my $pic =<<'EOPIC';
R0lGODlhHAAcAPcAAGMpIWMxJWs2K29CMWtOPXlMPYFaSoFoXYhzY4p/c4yGgZSJfpyMf6uQ
d5SMjJqUj6Scjq2glLOikrWlnL2pkLmtnMGxlMq1lM69lM69nNa9lNa9nNbGnN69lN7GlN7G
nLW1rb2xqb25tci9rdbGpd7GpdbOsc7Gvd7Opd7Ord7Otd7WtefOpefOrefWrefWtd7Ovd7W
vefWvefeve/evdbOxt7Wxufexu/exu/nxufezu/nzu/n1u/v1vfv1v///wAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACwAAAAAHAAcAAAI/gB/CBxIsKDBgwgTKjRooYLD
hw8nVJgQ4YGIhT8s2NBxw4ZHjzFqwIAhQgGEiwot3ODBsiWPHTdmzDCxoMFJhRVWunwZUybN
Bg0ehEiYc+cNFSZMqFCRYcGCBAoUENX50kaIBQcQMIhgQIJXCFIRVrDRMgaEAgICBBhAoMAF
DhosLEg4IQZLHRPQDhiQdi0GFCgwzEVY96UJBAUSUDygdsBfFE0TRoABs8IBCUknGGj8ODLC
CCdumJ1g4gWJAwX2Gug8+GAEEzNUvH7xAkYGDLgxfCjxIUNY1yZkyEhBYkOG2xoAkzjOwXdC
CMFNG9g7oMCBDSQkILDQ/LdBiytemmwYoFYAAgoXJBw4cKG394LgxZNXSwDBAQIEGOh2jvAB
CBUuZMCXWgEIIMAACFygQQcYJJCQfwBmUIB9fB24nQYYNvggCCmksMEBFlzAQAILQKBgBxk6
iJADHKaAwga7NYfhjCkmxCIJKJRQAgckcLDBBjRieIGKBzkgEURIOjTBkhAQ+Z0DUEYppZQK
OBAVRlhmqeWWXP4QEAAAOw==
EOPIC
	return $format,$pic;
}
