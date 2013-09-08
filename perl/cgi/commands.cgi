#!/usr/bin/perl 

#
# A CGI script that execute some scripts ...
#

$| = 1;

use Data::Dumper;
use SNMP;   # to stop hvps
use CGI; 
use IO::Socket::INET;
use Convert::Binary::C;
use IO::Socket::INET;

use constant MARKER                  => '#';
use constant PROTOCOL_VERSION        => 3;
use constant OPTION                  => 0x35;
use constant RESULT                  => 0x2;


use constant GET_CURRENT_CONFIG      => 0x16;
use constant GET_CONFIG              => 0x9;
use constant  SAVE_NEW_CONFIG         => 0x1c;
use constant  SAVE_CURRENT_CONFIG     => 0x17;

use constant  CMD_HW_TEST             => 0x08;
use constant  SUBCMD_INIT_HW          => 0x1e;
use constant  CMD_OFFSET_FRAME_COLLECT => 0x20;


use constant  SUB_CMD_ORDER_HW        => 0x19;
use constant  RESET_SEQ_CNTRS         => 0x0c; 
use constant  RESET_BUFFER_DRIVER_POINTERS => 0x09;
# -----------------------------------------------------
#
# Send a command to the application!
#
# -----------------------------------------------------


sub send_command_data_n {

my ($args) = @_;


# Set to default if not defined 

$args->{marker}  = MARKER unless defined($args->{marker});

$args->{version} = PROTOCOL_VERSION unless defined($args->{version});
$args->{command}  = 0x12 unless defined($args->{command});
$args->{option}  = OPTION unless defined($args->{option});
$args->{result}  = RESULT unless defined($args->{result});
$args->{cmd_tag}  = int(rand(0xffffffff)) unless defined($args->{cmd_tag});
$args->{length}  = 0x0  unless defined($args->{length});
$args->{data}   = 0x0  unless defined($args->{data});
$args->{data}   = 0x0  unless defined($args->{data});


  $Cmd = { version => $args->{version}, cmd => $args->{command} ,option => $args->{option}, result => $args->{result}, cmd_tag=> 
           $args->{cmd_tag}, length=> $args->{length} 
         };

  print "subcmd: " . $args->{subcmd};
  $Cmd->{specific}->{hw_test}->{sub_cmd} = $args->{subcmd};
  print "dw0: " . $args->{dw0};
  $Cmd->{specific}->{hw_test}->{dw}[0] = $args->{dw0};
  $Cmd->{specific}->{hw_test}->{dw}[1] = $args->{dw1};
  $Cmd->{specific}->{hw_test}->{dw}[2] = $args->{dw2};
  $Cmd->{specific}->{hw_test}->{dw}[3] = $args->{dw3};


  $pack = $args->{packet}->pack('Cmd', $Cmd);

  $args->{socket}->send($args->{marker});
  $args->{socket}->send($pack);

# data

for ($i= 0; $i < $args->{length}; $i++) {
 $n = pack "c", $args->{data}; 
 $args->{socket}->send($n);
# $args->{data}++;
}

}


sub send_command_data_p {

my ($args) = @_;


# Set to default if not defined 

$args->{marker}  = MARKER unless defined($args->{marker});

$args->{version} = PROTOCOL_VERSION unless defined($args->{version});
$args->{command}  = 0x12 unless defined($args->{command});
$args->{option}  = OPTION unless defined($args->{option});
$args->{result}  = RESULT unless defined($args->{result});
$args->{cmd_tag}  = int(rand(0xffffffff)) unless defined($args->{cmd_tag});
$args->{length}  = 0x0  unless defined($args->{length});
$args->{data}   = 0x0  unless defined($args->{data});
$args->{data}   = 0x0  unless defined($args->{data});


  $Cmd = { version => $args->{version}, cmd => $args->{command} ,option => $args->{option}, result => $args->{result}, cmd_tag=> 
           $args->{cmd_tag}, length=> $args->{length} 
         };

  $Cmd->{specific}->{offset_frame_collect}->{enable} = $args->{enable};  

  print Dumper($Cmd);

  $pack = $args->{packet}->pack('Cmd', $Cmd);

  $args->{socket}->send($args->{marker});
  $args->{socket}->send($pack);

# data

for ($i= 0; $i < $args->{length}; $i++) {
 $n = pack "c", $args->{data}; 
 $args->{socket}->send($n);
# $args->{data}++;
}

}


#-------------------------------------------------------------------------------
# Command()
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : 
#                
# Input        : -
# Return       : 
#-------------------------------------------------------------------------------

sub recv_hwtest {

$s = shift;
$c = shift;

$$s->read($reply,1);     # read marker!


$Reply = $$c->sizeof('Cmd');

$$s->read($reply,$Reply);

$x1= $$c->unpack('Cmd', $reply);

#print Dumper($x1);


return;

 print "Cmd.version = $x1->{version}\n";
 print "Cmd.cmd = $x1->{cmd}\n";
 print "Cmd.option = $x1->{option}\n";
# print "Cmd.cmd_tag = $x1->{cmd_tag}\n";
 printf ("Cmd.cmd_tag =%lx\n", $x1->{cmd_tag});
 printf ("Cmd.length =%lx\n", $x1->{length});
 printf ("Cmd.result =%lx\n", $x1->{result});
# printf ("sub_cmd =%x\n", $x1->{specific}->{hw_test}->{sub_cmd});
 print "sub_cmd:$x1->{specific}->{hw_test}->{sub_cmd}\n";

 printf ("dw0 =%lx\n", $x1->{specific}->{hw_test}->{dw}[0]);
 printf ("dw1 =%lx\n", $x1->{specific}->{hw_test}->{dw}[1]);
 printf ("dw2 =%lx\n", $x1->{specific}->{hw_test}->{dw}[2]);
 printf ("dw3 =%lx\n", $x1->{specific}->{hw_test}->{dw}[3]);

}


# -----------------------------------------------------
#
# Stop the high voltage supply promptly.
#
# -----------------------------------------------------

sub stop_hvps($) {

 $h = shift;

$s = new SNMP::Session(DestHost => $h,
                          Community => 'guru',
                          Version   => 2,) or die "help";

$tmp = SNMP::addMibFiles("./WIENER-CRATE-MIB.txt");

# Stop it!
$s->set('sysMainSwitch.0', 0);

 print "$h stopped!\n";
}


# -----------------------------------------------------
#
# Init HW!
#
# -----------------------------------------------------

sub init_hw() {

  my $c = eval {
   Convert::Binary::C->new(Include => ['.'])
                      ->parse_file('cs_commands.h')
  };
 if ($@) {
    print "Hey something went wrong1!";
  }



 $sock = IO::Socket::INET->new(PeerAddr => '127.0.0.1',
                                 PeerPort => '4390',
                                 Proto    => 'tcp') or die "no socket";


 send_command_data_n({ socket  =>$sock,
                      packet  => $c,
                      command => CMD_HW_TEST,
                      subcmd => SUBCMD_INIT_HW,                      
                      dw0 => 0,
                      dw1 => 0,
                      dw2 => 0,
                      dw3 => 0,
                      length  => 0x0,
                      data    => 22 
                   });


 recv_hwtest(\$sock, \$c); 
 
print "init_hw excuted\n";
}


# -----------------------------------------------------
#
# set CS in single shot
#
# -----------------------------------------------------

sub rem_single() {

 system("/opt/css/cli/cs_cmd -f /opt/css/cli/rem_single.cfg 2> &1");
 print "rem_single excuted\n";
}

# -----------------------------------------------------
#
# Reset seq cntrs
#
# -----------------------------------------------------

sub reset_seqcntrs () {


  my $c = eval {
   Convert::Binary::C->new(Include => ['.'])
                      ->parse_file('cs_commands.h')
  };
 if ($@) {
    print "Hey something went wrong1!";
  }



 $sock = IO::Socket::INET->new(PeerAddr => '127.0.0.1',
                                 PeerPort => '4390',
                                 Proto    => 'tcp') or die "no socket";


 send_command_data_n({ socket  =>$sock,
                      packet  => $c,
                      command => CMD_HW_TEST,
                      subcmd => SUB_CMD_ORDER_HW,                      
                      dw0 => RESET_SEQ_CNTRS,
                      dw1 => 0,
                      dw2 => 0,
                      dw3 => 0,
                      length  => 0x0,
                      data    => 22 
                   });


recv_hwtest(\$sock, \$c); 
 
print "reset seq cntrs excuted\n";


}

# -----------------------------------------------------
#
# Reset driver pointers
#
# -----------------------------------------------------

sub reset_ptrs() {

  my $c = eval {
   Convert::Binary::C->new(Include => ['.'])
                      ->parse_file('cs_commands.h')
  };
 if ($@) {
    print "Hey something went wrong1!";
  }



 $sock = IO::Socket::INET->new(PeerAddr => '127.0.0.1',
                                 PeerPort => '4390',
                                 Proto    => 'tcp') or die "no socket";


 send_command_data_n({ socket  =>$sock,
                      packet  => $c,
                      command => CMD_HW_TEST,
                      subcmd => SUB_CMD_ORDER_HW,                      
                      dw0 => RESET_BUFFER_DRIVER_POINTERS,
                      dw1 => 0,
                      dw2 => 0,
                      dw3 => 0,
                      length  => 0x0,
                      data    => 22 
                   });


recv_hwtest(\$sock, \$c); 
 
print "reset buffer pointer excuted\n";


}

# -----------------------------------------------------
#
# start collect offset frames
#
# -----------------------------------------------------

sub collect {

  $enable = shift;

  my $c = eval {
   Convert::Binary::C->new(Include => ['.'])
                      ->parse_file('cs_commands.h')
  };
 if ($@) {
    print "Hey something went wrong1!";
  }



 $sock = IO::Socket::INET->new(PeerAddr => '127.0.0.1',
                                 PeerPort => '4390',
                                 Proto    => 'tcp') or die "no socket";



  print "enable:$enable!";

 send_command_data_p({ socket  =>$sock,
                      packet  => $c,
                      command => CMD_OFFSET_FRAME_COLLECT,
                      enable => $enable,
                      length  => 0x0,
                   });


recv_hwtest(\$sock, \$c); 
 
print "excuted\n";


}


# -----------------------------------------------------
#
# Save local copy of offset frame in current configuration(and file)
#
# -----------------------------------------------------

sub lcopy {

  $lcopy = shift;

  my $c = eval {
   Convert::Binary::C->new(Include => ['.'])
                      ->parse_file('cs_commands.h')
  };
 if ($@) {
    print "Hey something went wrong1!";
  }



 $sock = IO::Socket::INET->new(PeerAddr => '127.0.0.1',
                                 PeerPort => '4390',
                                 Proto    => 'tcp') or die "no socket";



  print "enable:$enable!";

 send_command_data_l({ socket  =>$sock,
                      packet  => $c,
                      command => CMD_OFFSET_FRAME_COLLECT,
                      lcopy => $lcopy,
                      length  => 0x0,
                   });


recv_hwtest(\$sock, \$c); 
 
print "excuted\n";


}


# Main




$q = new CGI;                        # create new CGI object
print $q->header(),                    # create the HTTP header
      $q->start_html($ENV{SERVER_NAME}), # start the HTML
      $q->start_form(),
      $q->h1('Commands to ' . $ENV{SERVER_NAME}),
      $q->submit(-name=>'stop',-value=>'Stop HVPS'),
      $q->popup_menu(-name=>'color',
                 -values=>['192.168.2.180','192.168.2.181']),
      $q->br,
      $q->submit(-name=>'init_HW',-value=>'Init HW'),
      $q->br,
      $q->submit(-name=>'rem',-value=>'rem single'),
      $q->br,
      $q->submit(-name=>'rstseq',-value=>'reset seq cntrs'),
      $q->br,
      $q->submit(-name=>'rstptrs',-value=>'reset driver buffer counters'),
      $q->br,
      $q->submit(-name=>'collect',-value=>'start offset frame collection'),
      $q->br,
      $q->submit(-name=>'nocollect',-value=>'stop offset frame collection'),
      $q->br,
      $q->submit(-name=>'lcopy',-value=>'save local offset_frame'),
      $q->br,
#      $q->submit(-name=>'return',-value=>'return'),

      $q->end_form();

 if ($q->param()) {

     my $stop      = $q->param('stop');
     my $hvps      = $q->param('color');
     my $init_HW      = $q->param('init_HW');
     my $rem      = $q->param('rem');
     my $rstseq      = $q->param('rstseq');
     my $rstptrs      = $q->param('rstptrs');
     my $collect      = $q->param('collect');
     my $nocollect      = $q->param('nocollect');
     my $lcopy       = $q->param('lcopy');
     my $return      = $q->param('return');

     if (defined($stop)) { stop_hvps($hvps)};
     if (define