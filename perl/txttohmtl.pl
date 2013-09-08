#!/usr/bin/perl
# example: txttohtml *.txt
for $file (@ARGV) {
	open (INFILE, $file);
	open (OUTFILE, ">" . $file . ".html" );
	print OUTFILE '<pre>',"\n";
	while (<INFILE>) {
		s/&/&amp;/g;
		s/</&lt;/g;
		s/>/&gt;/g;
		s/"/&quot;/g;
		print OUTFILE;
	}
	print OUTFILE '</pre>',"\n";
	close INFILE;
	close OUTFILE;
}
