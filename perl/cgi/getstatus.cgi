#!/usr/bin/perl 

#
# A CGI script that execute some scripts ...
#


use Switch;
use SNMP;   # to stop hvps
use CGI qw/:push -nph/; 

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
use constant GET_STATUS               => 0x6; 


use constant  CMD_HW_TEST             => 0x08;
use constant  SUBCMD_INIT_HW          => 0x1e;

use constant  SUB_CMD_ORDER_HW        => 0x19;
use constant  RESET_SEQ_CNTRS         => 0x0c; 
use constant  RESET_BUFFER_DRIVER_POINTERS => 0x09;

%system_state = (

  1 => 'INIT',
  2 => 'STANDBY',
  3 => 'WARMUP',
  4 => 'CHILL',
  5 => 'OPERATIONAL',
  6 => 'FAILED',
  7 => 'REDUCED'
);

%hvps_status = (
  0x0 => 'OK',     
  0xaa => 'HVPS 2 big diff',
  0xbb => 'HVPS power off'
);

%temps = (
  0 => 'OK'
);


sub translate {

$x = shift;

if(defined($temps{$x})) {
  return $temps{$x};
}
else {
  return $x;
}


}


sub sw_status {

$x = shift;

$i = 1;

foreach (0 .. 31) { 
  switch($x&$i) {

    case 0x1 { print "failure echo communication "}
    case 0x2 { print "corrupt configuration "}
    case 0x4 { print "failure configuration file "}
    case 0x8 { print "no driver contact hw_server "}
    case 0x10 { print "hvps power off "}
    case 0x20 { print "no hvps contact 1 "}
    case 0x40 { print "no hvps contact 2 "}
    case 0x80 { print "no hvps contact 3 "}
    case 0x100 { print "timeout at chill "}
    case 0x200 { print "timeout at warmup "}
    case 0x400 { print "no driver contact frame_grabber "}
      else { }
  }
  $i = $i << 1;
#  $i <<=  1;

}

}

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
$args->{dw0}   = 0x0  unless defined($args->{dw0});
$args->{dw1}   = 0x0  unless defined($args->{dw1});
$args->{dw2}   = 0x0  unless defined($args->{dw2});
$args->{dw3}   = 0x0  unless defined($args->{dw3});


  $Cmd = { version => $args->{version}, cmd => $args->{command} ,option => $args->{option}, result => $args->{result}, cmd_tag=> 
           $args->{cmd_tag}, length=> $args->{length} 
         };


  $Cmd->{specific}->{hw_test}->{sub_cmd} = $args->{subcmd};
#  print "dw0: " . $args->{dw0};
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


sub recv_get_status { 
 
$s = shift; 
$c = shift; 
 
$$s->read($reply,1);     # read marker! 
 
 
$Reply = $$c->sizeof('Cmd'); 
 
$$s->read($reply,$Reply); 
 
$x1= $$c->unpack('Cmd', $reply); 
 
#print Dumper($x1); 
 
 
 $data = $x1->{length}; 
# print "length:$data\n"; 
# printf ("expected:%d\n",$$c->sizeof('system_status_t')); 
 
# if (length($data) >= $$c->sizeof('system_status_t')) { 
# if ($data == $$c->sizeof('system_status_t')) 
  
    $olle = $$c->sizeof('system_status_t'); 
    $$s->read($reply,14);  
  
    my $perl = $$c->unpack('system_status_t', $reply); 
#    print Dumper($perl); 
#    printf ("software status =%lx\n", $perl->{software_status}); 

   print "software status ="; 
   print sw_status($perl->{software_status}) ."\n"; 
   print "cs temp =" . translate($perl->{cs_temperature}) ."\n"; 
   print "dc temp =" . translate($perl->{dc_temperature}) ."\n"; 
   print "mb temp =" . translate($perl->{mb_temperature}) ."\n"; 
   printf ("high_voltage=%s\n", $hvps_status{$perl->{high_voltage}});
   print "gas pressure =" . translate($perl->{gas_pressure}) ."\n";  
#   printf ("gas_pressure=%lx\n", $perl->{gas_pressure}); 
   printf ("system state=%s\n", $system_state{$perl->{system_state}}); 
  
 return; 
 
}

sub get_status() {

 
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
                      command => GET_STATUS, 
                      length  => 0x0, 
                      data    => 22  
                   }); 
 
 
recv_get_status(\$sock, \$c);  


}



# Main

$| = 1;

  print multipart_init(-boundary=>'----here we go!'); 
foreach (0 .. 4) { 
      print multipart_start(-type=>'text/plain');
#      print "The current status is now ";
      get_status(); 
      if ($_ < 4) { 
              print multipart_end; 
	    } else { 
              print multipart_final; 
	    } 
      sleep 1; 
    } 


   



__END__



 Todo:

 - return-button 

 - image on browser?

 - Push-feature for status on CS, get_status or INIT/STANDBY/OPERATIONAL/STANDBY!!


 
 print "Cmd.version = $x1->{version}\n"; 
 print "Cmd.cmd = $x1->{cmd}\n"; 
 print "Cmd.option = $x1->{option}\n"; 
# print "Cmd.cmd_tag = $x1->{cmd_tag}\n"; 
 printf ("Cmd.cmd_tag =%lx\n", $x1->{cmd_tag}); 
 printf ("Cmd.length =%lx\n", $x1->{length}); 
 printf ("Cmd.result =%lx\n", $x1->{result}); 
# printf ("sub_cmd =%x\n", $x1->{specific}->{hw_test}->{sub_cmd}); 


/* Problems detected by hw_server, bit 7-0 */
#define FAILURE_ECHO_COMMUNICATION                  (1<<0)
#define FAILURE_CORRUPT_CONFIGURATION               (1<<1)
#define FAILURE_CONFIGURATION_FILE                  (1<<2)
#define HS_NO_DEVICE_DRIVER_CONTACT                 (1<<3)
//#define HS_HVPS_POWER_OFF22                           (1<<4)
#define FAILURE_NO_HVPS_CONTACT_1                   (1<<4)
#define FAILURE_NO_HVPS_CONTACT_2                   (1<<5)
#define FAILURE_NO_HVPS_CONTACT_3                   (1<<6)
#define HS_TIMEOUT_AT_CHILL                         (1<<5)
#define HS_TIMEOUT_AT_WARMUP                        (1<<7)
/* Problems detected by frame_grabber bit 15-8*/
#define FG_NO_DEVICE_DRIVER_CONTACT                 (1<<8)
