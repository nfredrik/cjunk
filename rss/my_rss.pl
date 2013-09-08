#!/usr/bin/perl -w
  # A complete screen-scraper and RSS generator here:
 
use Env; 
use strict;
use XML::RSS::SimpleGen;
#my $url = q<http://www.dn.se/>;
#my $url = q<http://127.0.0.1/>;
# Get ip address from the quagga or similar...
my $url = q<http://127.0.0.1/>;

rss_new( $url, "quagga", "Gemini Control system" );
#rss_new( $url, $ENV{SERVER_NAME}, "Gemini Control system" );
rss_language( 'en' );
rss_webmaster( 'fredrik.svard@c-rad.se' );
rss_twice_daily();

rss_image("http://interglacial.com/rss/weebl.gif", 106, 140);

#get_url( $url );


# Extract error information from info_daa.log.0 and .1

## Generate a one or several html files under rss-directory
## use sequence number, saved in file earlier.

my $rss = "rss/proxy.html";
my $rss1 = "rss/proxy1.html";
my $rss2 = "rss/proxy2.html";
my $olle = "rss/olle.html";


# Add them to in the rss feed, hopefully they will turn up i the rss reader...

# arg 1 : path to htmlfilen
# arg 2 : raden där felet inträffade, 
# arg 3 : vet inte riktigt hur det används...
rss_item("$url$rss", "Title:Major problem with..", "Descripton: Hi how are you?");
rss_item("$url$rss1", "Title:Major problem1 with..", "Descripton: Hi how are you?");
rss_item("$url$rss2", "Title:Major problem2 with..", "Descripton: Hi how are you?");
rss_item("$url$olle", "Title:Va fan...", "Descripton: Hi how are you?");


#die "No items in this content?! {{\n$_\n}}\nAborting"
# unless rss_item_count();

rss_save( 'my_rss.rss', 45 );
exit;
