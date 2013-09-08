#!/usr/bin/perl 

#use locale;

use POSIX qw(locale_h);

open(FILE, "<concat.csv") or die "couldn't open file";
open(OUT, ">result.csv") or die "couldn't open file";


my $prev_line ="";

foreach $line (<FILE>) {

    if($line =~ /\d\,\d/) {
	print OUT $prev_line . $line;
    }
    else {
	chomp($line);
	$prev_line = $line;
    }
}
