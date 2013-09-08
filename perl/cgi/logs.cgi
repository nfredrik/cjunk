#!/usr/bin/perl -w
use CGI;                             # load CGI routines
$q = new CGI;                        # create new CGI object
print $q->header(),   # create the HTTP header
      $q->start_html($ENV{SERVER_NAME}), # start the HTML
      $q->h1('Log files on ' . $ENV{SERVER_NAME}),         # level 1 header
      $q->blockquote(
                     "* Latest application log",
                     $q->a({href=>"log0.cgi"},"here"),
                    ),

      $q->blockquote(
                     "* Older application log",
                     $q->a({href=>"log1.cgi"},"here"),
                    ),

      $q->blockquote(
                     "* Device driver log",
                     $q->a({href=>"driver.cgi"},"here"),
                    ),

      $q->blockquote(
                     "* All registers on CS",
                     $q->a({href=>"hw.cgi"},"here"),
                    ),

      $q->end_html;                  # end the HTML



__END__

       $q->hr;
