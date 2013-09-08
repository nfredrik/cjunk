#!/usr/bin/perl


$| = 1;

 print "Content-type: text/html\n\n";
 print "<tt>\n";


 foreach $key (sort keys(%ENV)) {
    print "$key = $ENV{$key}<p>";
 }

__END__


 foreach $line  (@lines)  {
   print "$line<p>";
}

print STDOUT $out;



unlink pelle;

 system "cat /proc/interrupts > pelle";



 open(FH,'pelle') or die "urg";

# @lines = split(/:/, <FH>);

while (<FH>) {
 chop;
 print"$_ <br>";
}

close FH;

