#!/usr/bin/perl -w
use strict;
use CGI;
my $q = new CGI;
my $name;
my $out;
$ENV{"PATH"} = "";
print $q->header(),
      $q->start_html('Oz Finger'),
      $q->h1('Oz Finger'),
      $q->start_form(),
      "Who do you want to finger? ",
      $q->textfield('name'),
      $q->submit('submit'),
      $q->p(),
      $q->end_form(),
      $q->hr();

if ($q->param()) {
    $name = $q->param('name');
    $name =~ s/[\s;\\\/]+//g;
    $out = `/usr/bin/finger $name`;
#    $out = `/bin/ls`;
    print $q->font({color => 'red'}),
    print $q->pre($q->font({color => 'red'}),$out),
          $q->hr();
}
print $q->end_html();

