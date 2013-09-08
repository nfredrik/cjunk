#!/usr/bin/perl 

#
# A CGI script that execute some scripts ...
#

$| = 1;

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
$args->{dw0}   = 0x0  unless defined($args->{dw0});
$args->{dw1}   = 0x0  unless defined($args->{dw1});
$args->{dw2}   = 0x0  unless defined($args->{dw2});
$args->{dw3}   = 0x0  unless defined($args->{dw3});


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



# Main

$q = new CGI;                        # create new CGI object


print $q->header(),                    # create the HTTP header
      $q->start_html($ENV{SERVER_NAME}), # start the HTML

   $q->end_html;                  # end the HTML


 if ($q->param()) {

     my $subcmd      = $q->param('subcmd');
     my $dw0      = $q->param('dw0');
     my $dw1      = $q->param('dw1');
     my $dw2      = $q->param('dw2');
     my $dw3      = $q->param('dw3');
     my $return      = $q->param('return');

     if (defined($subcmd)) { print "subcmd:" . $subcmd;};
     if (defined($dw0)) { print "dw0:" . $dw0;};
     if (defined($dw1)) { print "dw1:" . $dw1;};
     if (defined($dw2)) { print "dw2:" . $dw2;};
     if (defined($dw3)) { print "dw3:" . $dw3;};
     if (defined($return)) {  $q->redirect(-URL=> 'http://192.168.2.160/cgi-bin/index.cgi')};

 
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
                      subcmd => $subcmd,                       
                      dw0 => $dw0, 
                      dw1 => $dw1, 
                      dw2 => $dw2, 
                      dw3 => $dw3, 
                      length  => 0x0, 
                      data    => 22  
                   }); 
 
 
 recv_hwtest(\$sock, \$c);  

   }



__END__



 Todo:

 - return-button 

 - image on browser?

 - write register? from url best url?args?arg2   OR

 - Push-feature for status on CS, get_status or INIT/STANDBY/OPERATIONAL/STANDBY!!


      "What's your name? ",textfield('name'),p,
      "What's the combination?", p,
      checkbox_group(-name=>'words',
                     -values=>['eenie','meenie','minie','moe'],
                     -defaults=>['eenie','minie']), p,
      "What's your favorite color? ",
      popup_menu(-name=>'color',
                 -values=>['red','green','blue','chartreuse']),p,
