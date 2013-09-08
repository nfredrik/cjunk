#!/usr/bin/perl


#$| = 1;

 print "Content-type: text/html\n\n";
# print "<tt>\n";


open(FH,'/opt/css/appl/logs/info_log_daa.1') or die "urg";



while (<FH>) {
 chop;

 if($_ =~/ERROR/ || $_=~/FATAL/ || $_=~/ALERT/ || $_=~/CRIT/ || $_=~/WARN/) {
   print"<font color=\"red\" />$_ <br>";
 }

 elsif($_ =~/NOTICE/) {
   print"<font color=\"green\" />$_ <br>";
 }
 else {
   print"<font color=\"black\" />$_ <br>";
 }
}

close FH;

__END__

