#!/usr/bin/perl -w
use CGI qw/:standard -debug/;
$q = new CGI;                        # create new CGI object
#print $q->header('image.jpg'),   # create the HTTP header
print $q->header(),   # create the HTTP header
      $q->start_html($ENV{SERVER_NAME}), # start the HTML
#      $q->start_html(-head=>Link({-rel=>'shortcut icon', -href=>'http://192.168.2.160/c-rad-logo.ico'})),
#      $q->start_html(-head=>Link({-rel=>'next', -href=>'http://192.168.2.160/c-rad-logo.ico'})),
       $q->start_html(-head=>[Link({-rel=>'shortcut icon',
#                                    -type=>'image/png',
#                                    -href=>'http://192.168.2.160/c-rad-logo.ico'}),]),
#                                    -href=>'http://192.168.2.160/favicon.ico'}),]),
                                    -href=>'http://' . $ENV{SERVER_ADDR} .'/favicon.ico'}),]),

      $q->h1('Welcome to ' . $ENV{SERVER_NAME}),         # level 1 header
      $q->br('This is the web server on the control system. Its possible to read some log files and execute a limited number of commands.'), 
      $q->br('The application logs save events that goes on in user space. The device driver logs events in kernel space. '), 
      $q->blockquote(
                     "* Log files",
#                     $q->a({href=>"../index.html"},"here"),
#                     $q->a({href=>"../logs.html"},"here"),
                     $q->a({href=>"logs.cgi"},"here"),
                    ),

      $q->blockquote(
                     "* Commands",
                     $q->a({href=>"commands.cgi"},"here"),
                    ),

      $q->end_html;                  # end the HTML



__END__

       $q->hr;
