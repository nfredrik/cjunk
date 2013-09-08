#!/usr/local/bin/perl -w
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
#              CONVERT XML  P E R L  S C R I P T
#
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# FileName      : convert_xml.pl
# Author        : Fredrik Svard
# Date          : 20 September 2006
# Version       : 0.0.1
# Subjet        :
# Exit codes    : 0 = Success
#                 1 = Error
#                 2 = Warning
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Netcool/Proviso                                        Copyright(c) Telia
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# 2006-09-20 Fredrik Svard. First version.
# 2006-09-21 Fredrik Svard. Review.
# 2006-10-20 Fredrik Svard. Removed the last ~ in the formula_path
# 2007-01-20 Fredrik Svard  Corrected problem with more than one timestamps for different
#                           probes
# 2007-06-25 Fredrik Svard  New addrsss for KPI
#
# Following files are involved
#
# convertXML.pl - the script
# DTD file - for parsing
# counterfile - dbmfile to keep track of fail counters
# LogFile.pm  - support module for logging
# /etc/wgetrc - settings for wget
# log/wget.log - logfile for wget
# log/convertXML.log - logfile for convertXML.pl
# log/convertXML.log%yy%mm%dd-%HH:%MM:%SS - history log file, old convertXML.log saved when
# LOGMAX < size(convertXML.log)

#   Todo
# - Where to place .wgetrc?

use strict;   

#===============================================================================
# Constants
#===============================================================================
use constant MAXLOGIN        => 12;
use constant MAXINVALID      => 12;
use constant MAXFILEFAIL     => 12;
use constant MAXEMPTYFILE    => 12;
use constant MAXSAMERECEIVED => 12;
use constant MINUTE          => 60;
use constant HOUR            => ( MINUTE * 60 );

use constant ERR => 1;
use constant OK  => 0;

#use constant KPI_SERVER_ADDR => "131.177.123.97";
use constant KPI_SERVER_ADDR => "luckyluke.stadi.sonera.fi";
use constant PORT            => 443;
use constant USERNAME        => "TSSDN";
use constant PASSWORD        => "DNr2";

use constant LOGMAX      => 50000;
use constant VERSION     => "0.0.2";
use constant EXIT_STATUS => 0;

#===============================================================================
# Standard PERL modules
#===============================================================================
use File::Basename;
use File::Copy;
use Data::Dumper;
use Benchmark;
use File::stat;

#===============================================================================
# Specific PERL modules
#===============================================================================
use XML::LibXML;
use XML::Smart;
unshift( @INC, "/opt/datachannel/bulkadaptor/config/" )
  ;    # Prepend config directory, used for Logfile
use LogFile;
use lib qw(/opt/datachannel/bulkadaptor/locallib);
use SendMail;
use Time::Format qw(%time %strftime %manip);
use DBM::Deep;
use lib qw(/opt/datachannel/bulkadaptor/locallib);
use TranslateProbe qw(%TeliaSonera @Num_2_String);
use Fcntl qw(:flock);
use Tie::Hash::MultiValue;

#===============================================================================
# Global variables
#===============================================================================
my ($CONFIG_HOME) =
  File::Spec->catdir( '/opt', 'datachannel', 'bulkadaptor', 'config' );
my ($WORKING_HOME) =
  File::Spec->catdir( '/opt', 'datachannel', 'bulkadaptor', 'working' );
my ($DO_HOME) = File::Spec->catdir( '/opt', 'datachannel', 'bulkimport' );

#my ($DO_HOME)  = File::Spec->catdir( '/opt', 'datachannel', 'bulkadaptor', 'do');
my ($TMP_HOME)  = File::Spec->catdir( '/opt', 'datachannel', 'bulkadaptor', 'tmp');
my ($LOG_HOME) =
  File::Spec->catdir( '/opt', 'datachannel', 'bulkadaptor', 'log' );

my (@Xml_Files);
my ($FILE);

my ($DIRNAME) = File::Spec->canonpath( dirname($0) ) || dirname($0);
my ($COUNTERFILE) = File::Spec->catfile( $CONFIG_HOME, "counters" );
my ($RESULTFILE)  = File::Spec->catfile( $CONFIG_HOME, "badprobes" );
my ($WGETLOG)     = File::Spec->catfile( $LOG_HOME,    "wget.log" );

my ($PROGNAME) = basename( $0, ".pl" ) || basename($0);
my ($CURDIR)   = File::Spec->curdir();
my ($DEVNULL)  = File::Spec->devnull();
my ($VERBOSE)  = 1;
my ($DEBUG)    = 1;
my ($LOGFILE)  = File::Spec->catfile( $LOG_HOME, "$PROGNAME.log" );
my ($DTD)      = File::Spec->catfile( $CONFIG_HOME, 'kpi_dtd.xml' );
my ($PLOG);
my (%Remember_This);
my ($Time_Stamp) = "0";

my ($WGET) =
  File::Spec->catfile( File::Spec->catdir('/usr/local/bin'), 'wget' );
my (%Kpi_Xml_Data);

#-------------------------------------------------------------------------------
# still_running()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Check if the program still running. Since the flock do not
#                work we try this solution for a while ...
#
# Input        : None
# Return       : true - if running
#-------------------------------------------------------------------------------
sub still_running() {
    system("ls $WORKING_HOME/\*.pvline\* > /dev/null 2>&1");
    if ( $? != 0 ) {
        return 0;
    }
    return 1;
}

#-------------------------------------------------------------------------------
# still_running()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Check if the program already is running
#
# Input        : None
# Return       : true - if running
#-------------------------------------------------------------------------------
sub old_still_running() {
    open( SELF, "<", $0 ) or die "Cannot open $0 - $!";
    return ( ( flock( SELF, LOCK_EX | LOCK_NB ) ) );
}

#-------------------------------------------------------------------------------
# get_hour_resolution()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Get the time in secs on hour resolution
#
# Input        : time resolution in secs
# Return       : time resolution in hour
#-------------------------------------------------------------------------------
sub get_hour_resolution($) {
    my $x = shift;
    return ( int( $x / HOUR ) * HOUR );
}

#-------------------------------------------------------------------------------
# open_file_write_header()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Open a file and start write header info in PVline format
#
# Input        : file name
# Return       : file handle
#-------------------------------------------------------------------------------
sub open_file_write_header($) {
    my $file = shift;
    my $fh;
    open( $fh, ">$file" );

    print $fh "TYPE Both\n";
    print $fh "#\n";
    print $fh "#\n";
    print $fh "#Setting Options\n";
    print $fh "OPTION:Type=Line\n";
    print $fh "OPTION:PVMVersion=3.0\n";

    #    print $fh "OPTION:TimeOffset=0\n";
    print $fh "# Collection Interval for the following resources = 5 minutes\n";

    return $fh;
}

#-------------------------------------------------------------------------------
# close_file()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Close file
#
# Input        : file handle
# Return       : none
#-------------------------------------------------------------------------------
sub close_file($) {
    my $filehandle = shift;
    close($filehandle);
}

#-------------------------------------------------------------------------------
# translate_probe()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Translate abbreviations to City names
#
# Input        : -
# Return       : None
#-------------------------------------------------------------------------------
sub translate_probe($) {
    my $x = shift();
    if ( defined( $TeliaSonera{$x} ) ) {
        return ( $TeliaSonera{$x} );
    }
    else {
        return ($x);
    }
}

#-------------------------------------------------------------------------------
# ws_2_ul()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Substitutes all white space with underline
#
# Input        : -
# Return       : None
#-------------------------------------------------------------------------------
sub ws_2_ul($) {
    my $x = shift();
    $x =~ tr/\ /\_/;
    return $x;
}

#-------------------------------------------------------------------------------
# convert_2_sec()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Puts a period sign in the numeral string
#
# Input        : -
# Return       : None
#-------------------------------------------------------------------------------
sub convert_2_sec($) {
    my $x = shift();
    $x =~ s/([0-9]{10})([0-9]{3})(.*)/$1\.$2/;
    return $x;
}

#-------------------------------------------------------------------------------
# get_metric_type
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
# Input        : -
# Return       : 0 - No, 1 - Yes
#-------------------------------------------------------------------------------
sub get_metric_type($) {
    my $test = shift();

    if ( defined($test) ) {
        if ( $test =~ /(\-)*(\d|\.)+/ ) {
            return "float";
        }
        elsif ( $test =~ /(\w)+/ ) {
            return "string";
        }
        else {
            return undef;
        }
    }
    else {
        return undef;
    }
}

#-------------------------------------------------------------------------------
# get_directory
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
# Input        : -
# Return       : 0 - No, 1 - Yes
#-------------------------------------------------------------------------------
sub get_directory($) {
    my $test = shift();

    if ( $test =~ /[dD]+elay/ ) {
        return "Delay~";
    }
    elsif ( $test=~ /[lL]+atency/) {
        return "Delay~";
    }
    elsif ( $test =~ /[jJ]+itter/ ) {
        return "Jitter~";
    }
    elsif ( $test =~ /[lL]+ost/ ) {
        return "Loss~";
    }
    else {
        return "Misc~";
    }
}

#-------------------------------------------------------------------------------
# check_environment()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Check that everything in the environment is OK before connecting
#                to the KPI exchange
#
# Input        : -
# Return       : 0 - Success, 1 - Failure
#-------------------------------------------------------------------------------
sub check_environment() {
    my $result = 0;

    # Check the directories including log
    if (   check_directory( \$WORKING_HOME )
        || check_directory( \$DO_HOME ) )
    {

        $result = 1;
    }

    print "1 $result\n";

    # check the DTD, exists and have a non-zero size
    unless ( -s "$DTD" ) {
        $result = 1;
    }

    # check wget can execute
    unless ( -x "$WGET" ) {
        $result = 1;
    }
    print "2 $result\n";

    # Read the received a timestamp from KPI.

    unless ( dbmopen( %Remember_This, $COUNTERFILE, 0777 ) ) {
        $PLOG->PrintError("check_environment: Cannot open DBM $COUNTERFILE");
    }

    $PLOG->PrintInfo(
"check_environment: filecounter: $Remember_This{'filecounter'}, LoginFailCntr: $Remember_This{'loginfail'} ,filefail:$Remember_This{'filefail'}, InvalidXML:$Remember_This{'validfail'}, emptyXML:$Remember_This{'emptyfile'}, Received:$Remember_This{'received'} "
    );

    $Time_Stamp = $Remember_This{"received"};

    unless ( dbmclose(%Remember_This) ) {
        $PLOG->PrintError("check_environment: Cannot close DBM $COUNTERFILE");
    }

    return $result;
}

#-------------------------------------------------------------------------------
# leave_this()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Save $WGETLOG with date as suffix and exit!
#
# Input        : -
# Return       : 0 - Success, 1 - Failure
#-------------------------------------------------------------------------------
sub leave_this() {
    unless ( -f "$WGETLOG" ) {

        $PLOG->PrintError("leave_this: failed to find $WGETLOG");
        exit 0;
    }

    my $date = `date '+%y%m%d-%H:%M:%S'`;
    my $log  = "$WGETLOG" . ".$date";
    chomp($log);
    move $WGETLOG, $log or $PLOG->PrintError("leave_this: failed to move $!");
    chmod 0444, "$log";

    exit 0;
}

#-------------------------------------------------------------------------------
# check_size_logfile()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Check size of logfile.
#
# Input        : -
# Return       : 0 - Success, 1 - Failure
#-------------------------------------------------------------------------------
sub check_size_logfile() {
    unless ( -f "$LOGFILE" ) {
        SendMail::send_trap( "Faulty environment", "Can't find $LOGFILE" );
    }

    my $mod  = stat($LOGFILE);
    my $size = $mod->size;

    if ( $size > LOGMAX ) {

        my $date = `date '+%y%m%d-%H:%M:%S'`;
        my $old  = "$LOGFILE" . ".$date";
        chomp($old);
        copy $LOGFILE, $old or $PLOG->PrintError("check_size_logfile: $!");

        chmod 0555, "$old";
        `:>$LOGFILE`;    # A better way to create a file???
    }
}

#-------------------------------------------------------------------------------
# check_directory( )
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Check that the directory exists, if not create it. Check if
#                writable.
# Input        : -
# Return       : 0 - No, 1 - Yes
#-------------------------------------------------------------------------------
sub check_directory(\$) {

    my $dir = shift();

    # Check that $dir exists
    if ( ( not defined $$dir ) or ( not -d $$dir ) ) {
        return ERR;
    }
    else {
        my $st   = stat($$dir);
        my $mode = $st->mode;
        if ( $mode & 0200 ) {
        }
        else {
            return ERR;
        }
    }
    return OK;
}

#-------------------------------------------------------------------------------
# connect_2_kpi( )
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Login to KPI, if failure, return zero
#
# Input        : -
# Return       : 0 - No, 1 - Yes
#-------------------------------------------------------------------------------
sub connect_2_kpi() {

    my $url = "https://"
      . KPI_SERVER_ADDR . ":"
      . PORT
      . "/kpi/xml-data?"
      . "username="
      . USERNAME . "&"
      . "password="
      . PASSWORD . "&"
      . "received=$Time_Stamp";

    my $content      = "$$.xml";
    my $login_result = 1;
    my $timeout      = 30;
    my $file_suffix  = "*.xml";
    
   # return 0;  vhen faking ...

    $PLOG->PrintInfo("connect_2_kpi: Trying to connect to: $url");

    my $cmd = "$WGET"
      . " --debug "
      . " --timeout=$timeout "
      . " --no-check-certificate "
      . " --no-directories "
      . " --tries=1 "
      . " --server-response "
      . " --output-file=$WGETLOG "
      . " --output-document=$content "
      . "--directory-prefix=$WORKING_HOME "
      . "'$url'";

    unless ( open( FILE, "$cmd|" ) ) {

        $PLOG->PrintError("Failed to execute commande line: $cmd");
        $login_result = 1;
        return $login_result;
    }

    close(FILE) or $PLOG->PrintError("connect_2_kpi: $!");

    my $mod  = stat($content);
    my $size = $mod->size;
    $PLOG->PrintInfo("connect_2_kpi: filesize: $size");

    move $content, $WORKING_HOME
      or $PLOG->PrintError("connect_2_kpi: failed to move $!");

    # Examine the wgetlog-file!
    open( FILE, "<$WGETLOG" ) or $PLOG->PrintError("connect_2_kpi: $!");

    while (<FILE>) {
        if ( $_ =~ /HTTP\/1\.1\ 200\ OK/ ) {
            $login_result = 0;
            last;
        }
    }

    # Find time for the KPI-server
    while (<FILE>) {
        if ( $_ =~ /Date\:/ ) {
            chomp($_);
            if ( $_ =~ /.{11}(.*)/ ) {
                $PLOG->PrintInfo("connect_2_kpi: KPI-time : $1");
            }
            last;
        }
    }

    close(FILE) or $PLOG->PrintError("connect_2_kpi: $!");

    return $login_result;
}

#-------------------------------------------------------------------------------
# check_files( )
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
# Input        : -
# Return       : A list of filenames in @filenames
#-------------------------------------------------------------------------------

sub check_files() {
    my @files = ();

    system("ls $WORKING_HOME/\*.xml > /dev/null 2>&1");
    if ( $? != 0 ) {
        return ERR;
    }
    return OK;
}

#-------------------------------------------------------------------------------
# validate_xml_files( )
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Fetch XML files. Validate that the files have an XML valid syntax,
#                according to the file $DTD.
#                Return a list of @filenames to convert to PV-Line format
#
# Input        : -
# Return       : A list of filenames in @filenames
#-------------------------------------------------------------------------------

sub validate_xml_files() {
    my $i;
    my @files = ();

    # Parse a DTD provided by CreaNord
    my $dtd = XML::LibXML::Dtd->new( 'ignore', $DTD );

    # Validate files and and eventually put filenames in an array
    for $i (`ls $WORKING_HOME/*.xml `) {
        chomp($i);
        my $xml;

        eval { $xml = XML::LibXML->new->parse_file($i) };

        # Check error output from 'eval'
        if ( ($@) ) {
            $PLOG->PrintError("validate_xml_files: phase1 invalid file $i, $@");
            @files = ();
            return @files;
        }

        eval { $xml->validate($dtd) };

        # Check error output from 'eval'
        if ( ($@) ) {
            $PLOG->PrintError(
                "validate_xml_files2: phase2 invalid file $i, $@");
            @files = ();
            return @files;
        }
        push @files, $i;
    }
    return @files;
}

#-------------------------------------------------------------------------------
# convert_xml_file( )
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Convert the XML file to a PVLine file
#
# Input        : filename
# Return       : 0 - No, 1 - Yes
#-------------------------------------------------------------------------------

sub convert_xml_file($) {
    my $file = shift();
    my $XML  = XML::Smart->new("$file");
    my %probe_test_timestamps;
    tie %probe_test_timestamps, 'Tie::Hash::MultiValue';
    $XML = $XML->{'kpi-results'};    # set as 'root'
    my ($formula_path) = "PS~Bulk~Brix~";

    #my ($formula_path) = "AP~Specific~Bulk~Brix~";
    my ($time_offset) = 0;
    my ( $filename, $directories, $suffix ) = fileparse( $file, ".xml" );
    my $target_name; 
    my $date = `date '+%y%m%d-%H:%M:%S'`;
    chomp($date);

    my ($t0) = new Benchmark;
    my $db = DBM::Deep->new($RESULTFILE);

    # Check for 'empty' files!
    if ( "$XML->{kpi}{received}" ne "" ) {

        # Count number of path to iterate over
        my @number = $XML->{kpi}( '[@]', 'received' );
        my $many = scalar(@number);

        for ( my $i = 0 ; $i < $many ; $i++ ) {



                # If Nordic net, put an extra "_" to make the citypair unique
		if ( $XML->{kpi}[$i]{'kpi-descr'}{sn} =~ /Nordic/) {
		
                 $target_name =
                  translate_probe( $XML->{kpi}[$i]{'kpi-descr'}{source} ) . "__"
                  . translate_probe(
                    $XML->{kpi}[$i]{'kpi-descr'}{destination} );
		}
		else {
		$target_name =
                  translate_probe( $XML->{kpi}[$i]{'kpi-descr'}{source} ) . "_"
                  . translate_probe(
                    $XML->{kpi}[$i]{'kpi-descr'}{destination} );
		}
		
		    
		# Save all received variables for a timestamp in a multivalue hash   
		$probe_test_timestamps{ $XML->{kpi}[$i]{'kpi-descr'}{time} } =  $XML->{kpi}[$i]{received};

                $Kpi_Xml_Data{ $XML->{kpi}[$i]{received} }->{rcvd} =
                  "# received :$XML->{kpi}[$i]{received} ->"
                  . $time{
                    'yyyy/mm/dd hh:mm:ss:mmm',
                    convert_2_sec( $XML->{kpi}[$i]{received} )
                  }
                  . "\n";

                $Kpi_Xml_Data{ $XML->{kpi}[$i]{received} }->{src} =
                  "OPTION:Element="
                  . ws_2_ul( $XML->{kpi}[$i]{'kpi-descr'}{sn} ) . "\n";

                $Kpi_Xml_Data{ $XML->{kpi}[$i]{received} }
                  ->{connector} = "G"
                  . $time{ 'yyyy/mm/dd hh:mm:ss',
                    $XML->{kpi}[$i]{'kpi-descr'}{time} }
                  . " | Family | alias | $target_name | | inventory | "
                  . "Brix-Probe\n";

                $Kpi_Xml_Data{ $XML->{kpi}[$i]{received} }->{ss} = "G"
                  . $time{ 'yyyy/mm/dd hh:mm:ss',
                    $XML->{kpi}[$i]{'kpi-descr'}{time} }
                  . " | ss | alias | $target_name | | property |"
                  . ws_2_ul( $XML->{kpi}[$i]{'kpi-descr'}{ss} ) . "\n";

                $Kpi_Xml_Data{ $XML->{kpi}[$i]{received} }
                  ->{'content-tag'} = "G"
                  . $time{ 'yyyy/mm/dd hh:mm:ss',
                    $XML->{kpi}[$i]{'kpi-descr'}{time} }
                  . " | content-tag  | alias | $target_name | | property |"
                  . ws_2_ul( $XML->{kpi}[$i]{'kpi-descr'}{'content-tag'} )
                  . "\n";

                $Kpi_Xml_Data{ $XML->{kpi}[$i]{received} }->{module} =
                  "G"
                  . $time{ 'yyyy/mm/dd hh:mm:ss',
                    $XML->{kpi}[$i]{'kpi-descr'}{time} }
                  . " | module | alias | $target_name | | property | $XML->{kpi}[$i]{'kpi-descr'}{module}\n";

		if ( $XML->{kpi}[$i]{r}( 'n', 'eq', 'result' ) eq '0' ) {

		  # Printing metric part of the body
		  my @num_r = $XML->{kpi}[$i]{r}( '[@]', 'n' );
		  my $many_r = scalar(@num_r);

		  $Kpi_Xml_Data{ $XML->{kpi}[$i]{received} }->{'many'} =
		    $many_r;

		  for ( my $k = 0 ; $k < $many_r ; $k++ ) {

                    my $metric_type = get_metric_type( $XML->{kpi}[$i]{r}[$k] );
                    my $directory = get_directory( $XML->{kpi}[$i]{r}[$k]{n} );
                    my $abs_formula_path =
		      "$formula_path"
			. "$directory"
			  . "$XML->{kpi}[$i]{r}[$k]{n}";

                    if ( defined($metric_type) ) {
		      $Kpi_Xml_Data{ $XML->{kpi}[$i]{received} }
			->{'r'}->{$k} = "G"
                          . $time{
				  'yyyy/mm/dd hh:mm:ss',
				  $XML->{kpi}[$i]{'kpi-descr'}{time}
                          }
			    . " | $abs_formula_path | alias | $target_name | | $metric_type | $XML->{kpi}[$i]{r}[$k]\n";

                    }
		  }
		}
		else {

		  # result <> 0, Save only the result code
                    my $metric_type = get_metric_type( $XML->{kpi}[$i]{r}[-1] );  # -1, picks the last element in array
                    my $directory = get_directory( $XML->{kpi}[$i]{r}[-1]{n} );
                    my $abs_formula_path =
		      "$formula_path"
			. "$directory"
			  . "$XML->{kpi}[$i]{r}[-1]{n}";
		    $Kpi_Xml_Data{ $XML->{kpi}[$i]{received} }->{'many'} = 1;  # only result code to convert

                    if ( defined($metric_type) ) {
		      $Kpi_Xml_Data{ $XML->{kpi}[$i]{received} }
			->{'r'}->{0} = "G"
                          . $time{
				  'yyyy/mm/dd hh:mm:ss',
				  $XML->{kpi}[$i]{'kpi-descr'}{time}
				 }
			    . " | $abs_formula_path | alias | $target_name | | $metric_type | $XML->{kpi}[$i]{r}[-1]\n";

		    }
		      $db->{
			"$XML->{kpi}[$i]{'kpi-descr'}{source} $XML->{kpi}[$i]{'kpi-descr'}{destination}"
		       }->[ $XML->{kpi}[$i]{r}( 'n', 'eq', 'result' ) ]++;

		  }
        }

        my @list     = sort keys %probe_test_timestamps;
        my $old_time = $list[0];
        my $flag     = 1;
        my $fh;
        my $file_name;

	
        # The DataLoad Bulk need the data in timestamp order, sort it
        foreach my $timestamp (@list) {
	
	        
	
	  	foreach my $ke (@{$probe_test_timestamps{$timestamp}}) {
		
		       

            		my $hour = get_hour_resolution($timestamp);

            		# Check that we are within "the hour"
            		# otherwise close file
            		if ( $old_time < $hour ) {
                	$flag     = 1;
                	$old_time = $hour;
                	close_file($fh);
            		}

          	  # If this is the first event on an "new hour"
                  # open a new file and write a header
            	if ($flag) {
                	$file_name = $$ . ".pvline." . $time{ 'yymmdd-hh:mm', $hour };
                	$fh        = open_file_write_header("$directories/$file_name");
               	        $flag      = 0;
            	}
            	print $fh $Kpi_Xml_Data{$ke}->{rcvd};
            	print $fh $Kpi_Xml_Data{$ke}->{src};
            	print $fh $Kpi_Xml_Data{$ke}->{connector};
            	print $fh $Kpi_Xml_Data{$ke}->{ss};
            	print $fh $Kpi_Xml_Data{$ke}->{'content-tag'};
            	print $fh $Kpi_Xml_Data{$ke}->{module};
            	for ( my $k = 0 ; $k < $Kpi_Xml_Data{$ke}->{many} ; $k++ ) {

                	if ( defined( $Kpi_Xml_Data{$ke}->{'r'}->{$k} ) ) {
                    	print $fh $Kpi_Xml_Data{$ke}->{'r'}->{$k};
                	}
            	}
       	    }
	 }
	 
        close_file($fh);

        my ($t1) = new Benchmark;

        # Save the receive-variable for next retrieve
        unless ( dbmopen( %Remember_This, $COUNTERFILE, 0777 ) ) {
            $PLOG->PrintError("convert_xml_file: Cannot open DBM $COUNTERFILE");
        }

        $PLOG->PrintInfo( "convert_xml_file: Process took : "
              . timestr( timediff( $t1, $t0 ) ) );

        my $the_time = convert_2_sec( $XML->{kpi}[ $many - 1 ]{received} );
        $PLOG->PrintInfo(
"convert_xml_file: Last received was :$XML->{kpi}[$many-1]{received} -> $time{'yyyy/mm/dd hh:mm:ss:mmm',$the_time}"
        );

        if ( $Remember_This{"received"} == $XML->{kpi}[ $many - 1 ]{received} )
        {

            $PLOG->PrintError(
"convert_xml_file: Paranoia, received variable is the same as it's successor!"
            );
        }

        $Remember_This{"received"} = $XML->{kpi}[ $many - 1 ]{received};

        # Keep track of no files beeing transferred.

        $Remember_This{"filecounter"}++;

        unless ( dbmclose(%Remember_This) ) {
            $PLOG->PrintError(
                "convert_xml_file: Cannot close DBM $COUNTERFILE");
        }
    }
    else {
        $PLOG->PrintError("convert_xml_file: Empty XML-file");

        # Remove files from $WORKING_HOME
        system("rm -f $WORKING_HOME/\*.xml");
        if ( $? != 0 ) {
	    $PLOG->PrintError("convert_xml_file: Couldn't remove Empty XML-file");
            return OK;
        }

        return OK;
    }

    return OK;
}

#-------------------------------------------------------------------------------
# move_xml_files( )
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Move converted files to directory known by Bulk DataLoad
#
# Input        : -
# Return       : 0 - No, 1 - Yes
#-------------------------------------------------------------------------------
sub move_xml_files() {

    # Move files to Bulk/Proviso directory
    system("mv $WORKING_HOME/\*.pvline\* $DO_HOME");
    if ( $? != 0 ) {
        return ERR;
    }

    #Remove files from $WORKING_HOME
    #system("mv  $WORKING_HOME/\*.xml $TMP_HOME");
    system("rm -f $WORKING_HOME/\*.xml");
    if ( $? != 0 ) {
        return ERR;
    }

    return OK;
}

#-------------------------------------------------------------------------------
# limit_exceeded( )
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Read a file that keep track of failed logins....
#
# Input        : -
# Return       : 0 - No, 1 - Yes
#-------------------------------------------------------------------------------
sub limit_exceeded($) {
    my $arg        = shift();
    my $returnCode = 0;

    unless ( dbmopen( %Remember_This, $COUNTERFILE, 0777 ) ) {
        $PLOG->PrintError("limit_exceeded: Cannot open DBM $COUNTERFILE");
    }

    if ( $arg eq "loginfail" ) {
        $Remember_This{"loginfail"}++;
        $Remember_This{"rloginfail"}++;

        if ( $Remember_This{"loginfail"} > MAXLOGIN ) {

            # Start it all over again ...
            $Remember_This{"loginfail"} = 0;
            $returnCode = 1;
        }
    }
    elsif ( $arg eq "validfail" ) {
        $Remember_This{"validfail"}++;
        $Remember_This{"rvalidfail"}++;

        if ( $Remember_This{"validfail"} > MAXINVALID ) {

            # Start it all over again ...
            $Remember_This{"validfail"} = 0;
            $returnCode = 1;
        }

    }
    elsif ( $arg eq "filefail" ) {
        $Remember_This{"filefail"}++;
        $Remember_This{"rfilefail"}++;

        if ( $Remember_This{"filefail"} > MAXFILEFAIL ) {

            # Start it all over again ...
            $Remember_This{"filefail"} = 0;
            $returnCode = 1;
        }

    }
    elsif ( $arg eq "emptyfile" ) {
        $Remember_This{"emptyfile"}++;
        $Remember_This{"remptyfile"}++;

        if ( $Remember_This{"emptyfile"} > MAXEMPTYFILE ) {

            # Start it all over again ...
            $Remember_This{"emptyfile"} = 0;
            $returnCode = 1;
        }

    }
    else {
        $PLOG->PrintError("limit_exceeded: wrong argument");
    }

    unless ( dbmclose(%Remember_This) ) {
        $PLOG->PrintError("limit_exceeded: Cannot close DBM $COUNTERFILE");
    }

    return $returnCode;
}

#-------------------------------------------------------------------------------
# clear_counter
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Clears error counter
#
# Input        : -
# Return       : 0 - No, 1 - Yes
#-------------------------------------------------------------------------------
sub clear_counter($) {
    my $arg = shift();

    unless ( dbmopen( %Remember_This, $COUNTERFILE, 0777 ) ) {
        $PLOG->PrintError("clear_counter: Cannot open DBM $COUNTERFILE");
    }

    $Remember_This{$arg} = 0;

    unless ( dbmclose(%Remember_This) ) {
        $PLOG->PrintError("clear_counter: Cannot close DBM $COUNTERFILE");
    }
}

#===============================================================================
#
#                                    M A I N
#
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
# Return value :
#   0 - Succeeded
#   1 - Failed
#   2 - Warning
#===============================================================================

#-------------------------------------------------------------------------------
# Benchmark initialisation
#-------------------------------------------------------------------------------
my ($t0) = new Benchmark;

#-------------------------------------------------------------------------------
# Initialize the log file, check size, create a new if to big
#-------------------------------------------------------------------------------
$PLOG =
  new LogFile( "$LOGFILE", ( LOGMAX == 0 ) ? 'true' : 'false', LOGMAX,
    $VERBOSE );
check_size_logfile();

#-------------------------------------------------------------------------------
# Prevent to start a 2nd instance of the program
#-------------------------------------------------------------------------------
if ( still_running() ) {

    $PLOG->PrintError("main: previous program still running, quit");
    exit 0;
}
else {
    $PLOG->PrintInfo("main: everything looks ok :-)");
}

#-------------------------------------------------------------------------------
# Check directories, wget and the sync with KPI, and existence of a DTD file
#-------------------------------------------------------------------------------
$PLOG->PrintInfo("main: Process starts, check environment ");

if ( check_environment() ) {
    $PLOG->PrintError("main: environment not correctly configured");
    SendMail::send_trap( "Faulty environment", "Please check" );
    leave_this();
}

#-------------------------------------------------------------------------------
# Fetch files from KPI EXCHANGE with wget. Trap to NetCool in case of failure, set
# MAXLOGIN
#-------------------------------------------------------------------------------
$PLOG->PrintInfo("main: Connect to KPI...");

if ( connect_2_kpi() ) {

    # If we failed MAXLOGIN times, send a trap to NetCool/NOC
    if ( limit_exceeded("loginfail") ) {
        $PLOG->PrintError(
            "main: Exceeded number of login attempts, sending trap to NetCool!"
        );
        SendMail::send_trap( "FailedLogin", "Number login attempt exceeded" );
    }
    $PLOG->PrintError("main: Failed to connect to KPI exchange");    #

    # Remove invalid XML files
    system("rm -f $WORKING_HOME/\*.xml");
    leave_this();
}
else {
    clear_counter("loginfail");
}

#-------------------------------------------------------------------------------
# Check that files have been fetched
#-------------------------------------------------------------------------------
$PLOG->PrintInfo("main: Check retrieved files ...");

if ( check_files() ) {

    # If we failed MAXLOGIN times, send a trap to NetCool/NOC
    if ( limit_exceeded("filefail") ) {

        $PLOG->PrintError(
            "main: No XML files fetched, sending trap to NetCool!");
        SendMail::send_trap( "FailedLogin", "No XML files fetched" );
    }
    $PLOG->PrintError("main: No XML files fetched");
    leave_this();
}
else {
    clear_counter("filefail");
}

#-------------------------------------------------------------------------------
# Validate XML-files.Trap to NetCool in case of  invalid XML files
# MAXINVALID times?
#-------------------------------------------------------------------------------
$PLOG->PrintInfo("main: Start validating the files");

@Xml_Files = validate_xml_files();

if ( scalar(@Xml_Files) < 1 ) {

    $PLOG->PrintError("main: Failed to parse  XML file(s)");

# Invalid XML file, if it exceed a MAXINVALID number of times , send a trap to NetCool/NOC
    if ( limit_exceeded("validfail") ) {

        $PLOG->PrintError(
"main: Exceeded number of attempts to validate files, sending trap to NetCool!"
        );
        SendMail::send_trap( "FailedValidate", "Invalid XML files from KPI..." );
    }

    # Remove invalid XML files
    system("mv $WORKING_HOME/\*.xml $LOG_HOME");
    leave_this();
}
else {

    clear_counter("validfail");

   #    $PLOG->PrintInfo("main: Number of fetched files:" . scalar(@Xml_Files));
}

#-------------------------------------------------------------------------------
# Convert files to PVLine file  $WORKING_HOME
#-------------------------------------------------------------------------------

$PLOG->PrintInfo("main: Start converting files");
for $FILE (@Xml_Files) {

    if ( convert_xml_file($FILE) ) {
        $PLOG->PrintError(
            "main: Didn't manage to convert file $FILE to pvline");

# Empty XML file, if it exceed a MAXEMPTYFILE number of times , send a trap to NetCool/NOC
        if ( limit_exceeded("emptyfile") ) {

            $PLOG->PrintError(
"main: Exceeded number of attempts to accept empty files, sending trap to NetCool!"
            );
            SendMail::send_trap( "FailedEmptyFiles", "Empty XML files from KPI..." );
        }

        leave_this();
    }
    else {
        clear_counter("emptyfile");
    }

}
$PLOG->PrintInfo("main: All files converted to pvline");

# Everything working well so far, remove wgetlogfile
system("rm -f $WGETLOG");

#-------------------------------------------------------------------------------
# Move converted files from $WORKING_HOME to $DO_HOME, known by
# Proviso/Bulk DataLoad
#-------------------------------------------------------------------------------

$PLOG->PrintInfo("main: Move all files ... ");
if ( move_xml_files() ) {
    $PLOG->PrintError("main: Failed to move files to $DO_HOME");
}
$PLOG->PrintInfo("main: All files moved to $DO_HOME");

#-------------------------------------------------------------------------------
# Exit
#-------------------------------------------------------------------------------

my ($t1) = new Benchmark;
$PLOG->PrintInfo( "main: Process took : " . timestr( timediff( $t1, $t0 ) ) );

exit EXIT_STATUS;

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# END OF SCRIPT
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
