#!/usr/bin/perl -w
use HTML::TextToHTML;

my $text_file ='proxy.txt';
my $html_file ='proxy.html';

# create a new object
my $conv = new HTML::TextToHTML();

# convert a file
$conv->txt2html(infile=>[$text_file], outfile=>$html_file,
                 title=>"Wonderful Things",
                 mail=>1,
  );

# reset arguments
$conv->args(infile=>[], mail=>0);

# convert a string
#$newstring = $conv->process_chunk($mystring)
