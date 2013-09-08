#!/usr/bin/perl -w
use IO::Socket::INET;
# -----------------------------------------------------
#
# Send a command to the application!
#
# -----------------------------------------------------

# Main
 $sock = IO::Socket::INET->new(PeerAddr => '127.0.0.1',
			       PeerPort => '4390', 
                                 Proto    => 'tcp') or die "no socket"; 

while(1) {
  $sock->send("Susanne var\n\0");
}

__END__
