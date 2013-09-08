#!/usr/bin/perl


#$| = 1;

 print "Content-type: text/html\n\n";

unlink output_hw_cgi;

system "cat /proc/plx_drv > output_hw_cgi";

 open(FH,'output_hw_cgi') or die "urg";



while (<FH>) {
 chop;
 print"$_ <br>";
}

close FH;

__END__

