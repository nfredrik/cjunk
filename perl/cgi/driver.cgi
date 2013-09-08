#!/usr/bin/perl


#$| = 1;

 print "Content-type: text/html\n\n";
# print "<tt>\n";

unlink output_driver_cgi;

# system "cat /proc/interrupts > output_driver_cgi";
 system "dmesg > output_driver_cgi";



 open(FH,'output_driver_cgi') or die "urg";



while (<FH>) {
 chop;

 if($_ =~/ERROR/) {
   print"<font color=\"red\" />$_ <br>";
 }
 else {
   print"<font color=\"black\" />$_ <br>";
 }
}

close FH;

__END__

