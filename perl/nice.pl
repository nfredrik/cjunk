#!/usr/bin/perl
use strict;
use Tk;
use Tk::HList;
use Tk::BrowseEntry;

my $sl;
my $report;

my $mw = MainWindow->new;

# Mainwindow: sizex/y, positionx/y
#$mw->geometry("500x200+100+120");
$mw->geometry("600x200+100+120");


# Default value
&create_datagrid($mw);
MainLoop;

sub bye_action {

    if ($sl) {
       $report->configure(-text => "Start");
        $sl = 0;
    } else {
       $report->configure(-text => "Stop");
        $sl = 1;
    }


}

sub create_datagrid {
    my $mother = shift;

    my @headers = ( "Financial status", "Name", "City", "Phone" );
    my @customers = (
        [ 'bad',  'Richard', 'Nuernberg', '123' ],
        [ 'good', 'Roland',  'Fuerth',    '586' ],
        [ 'fair', 'Peter',   'Zirndorf',  '933' ],
    );

    # Create dropdown to choose customer
    my $dropdown_value ='192.168.2.160';
    my $dropdown = $mother->BrowseEntry(
        -label    => "Choose Customer",
        -variable => '192.168.2.160',
    )->grid(
	    -column => 0, -row => 0,
    		-sticky => 'n');
    # Populate dropdown with values
#    foreach ( @customers ) {
    foreach ( ('192.168.2.160','192.168.2.161') ) {
        $dropdown->insert( 
		'end', 
		# value is the name of the customer
#		$_->[1],
		$_,
	);
    }

    my $grid = $mother->Scrolled(
        'HList',
        -head       => 1,
        -columns    => scalar @headers,
        -scrollbars => 'e',
        -width      => 40,
        -height     => 10,
        -background => 'white',
    )->grid(-column => 1, -row => 0,
      );

    my @headers = ( "Financial status", "Name", "City", "Phone" );
    foreach my $x ( 0 .. $#headers ) {
        $grid->header(
            'create',
            $x,
            -text             => $headers[$x],
            -headerbackground => 'gray',
        );
    }

    foreach my $row_number ( 0 .. $#customers ) {
	my $unique_rowname = $customers[$row_number]->[1];
        $grid->add($unique_rowname);
        foreach my $x ( 0 .. 3 ) {
            $grid->itemCreate( $unique_rowname, $x,
                -text => $customers[$row_number]->[$x] );

            # You can change gridcontent later
            $grid->itemConfigure( $unique_rowname, $x, -text => "don't care" )
              if rand > 0.5 and $x == 0;
        }
    }

    # Set the default selection, the name of the last customer
    my $unique_rowname = $customers[-1]->[1];
    $grid->selectionSet($unique_rowname);

    # Set the initial value for the dropdown
    # i.e. the last customer
    $dropdown_value = $customers[-1]->[1];
    
    # Configure dropdown what to do when sth is selected
    $dropdown->configure(
        # What to do when an entry is selected
        -browsecmd => sub {
            foreach ( 0 .. $#customers) {
		    # $grid->selectionClear( $customers[$_]->[1] );
		    $grid->selectionClear();
		}
            $grid->selectionSet( $dropdown_value );
        },
    );


$report = $mother->Button(
	-text => "Start",
	# call a sub somewhere
	-command => \&bye_action,
	)->grid(-column => 0, -row => 1,
      );




}



__END__

$mw->Button(
	-text => "Bye call a sub",
	# call a sub somewhere
	-command => \&bye_action,
	)->pack;
