#!/usr/bin/perl
  use CGI qw/:push -nph/;
  $| = 1;
  print multipart_init(-boundary=>'----here we go!');
  foreach (0 .. 4) {
      print multipart_start(-type=>'text/plain'),
            "The current time is ",scalar(localtime),"\n";
      if ($_ < 4) {
              print multipart_end;
      } else {
              print multipart_final;
      }
      sleep 1;
  }