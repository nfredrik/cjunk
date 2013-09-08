#!/usr/bin/perl -w

#
# Todo: how to handle several cases, i.e. ERRORs ??
# 
#
#

use constant LAST_LINES => 5;


my @var;


open(FILE, "<proxy.txt") or die "couldn't open file";




while (<FILE>) {


 if($#var < LAST_LINES) {
    push @var, $_;
    shift @var   if($#var == LAST_LINES);
 }

 if ($_ =~ /ERROR/) {
   print "@var";
   @var = ();
#   last;
 }

}







close(FILE);

__END__

print "$last";
