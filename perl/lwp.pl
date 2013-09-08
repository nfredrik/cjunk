#!/usr/bin/perl -w
# Create a user agent object
use LWP::UserAgent;
use GD;

my $vecka;

 $ua = LWP::UserAgent->new;
 $ua->agent("MyApp/0.1 ");

 # Create a request
 my $req = HTTP::Request->new(POST => 'http://www.vecka.nu');
 $req->content_type('application/x-www-form-urlencoded');
 $req->content('query=libwww-perl&mode=dist');

 # Pass request to the user agent and get a response back
 my $res = $ua->request($req);

 # Check the outcome of the response
 if ($res->is_success) {
#     print $res->content;
 }
 else {
     print $res->status_line, "\n";
 }

$test= $res->content;

@row = split("\n", $test);


foreach(@row) {

 chomp;

# if($_ ~= /(.*)family(.*)\>(\d)+/)
 next until /bold\;\"\>(\d+)/;
# print "vecka:$1\n";

$vecka = $1;

}


## Make an image

    # create a new image
    $im = new GD::Image(140,25);

 # allocate some colors
    $white = $im->colorAllocate(255,255,255);
    $black = $im->colorAllocate(0,0,0);       
    $red = $im->colorAllocate(255,0,0);      
    $blue = $im->colorAllocate(0,0,255);

    # make the background transparent and interlaced
#    $im->transparent($white);
#    $im->interlaced('true');

    # Put a black frame around the picture
#    $im->rectangle(0,0,99,99,$black);

    # Draw a blue oval
#    $im->arc(50,50,95,75,0,360,$blue);

    # And fill it with red
#    $im->fill(50,50,$red);

    $im->string(gdSmallFont,2,10,"Nu ar det vecka: $vecka" ,$black);        

    # make sure we are writing to a binary stream
    binmode STDOUT;

    # Convert the image to PNG and print it to a file

    open FH, ">/tmp/file.png" or die "can open a file";
    binmode(FH);
    print FH $im->png;

    close FH;


__END__

 ej    :<html>
 ej    :<head>
 ej    :<title>vecka.nu</title>
 ej    :</head>
 ej    :<body bgcolor="#90C0DE" text="#066EB0">
 ej    :<table width="100%" height="100%">
hej    : <tr><td valign="top" align="left"><a target="_blank" href="http://www.paregos.com"><img src="love.gif" border="0"></ ></td></tr>
 ej    : <tr>
 ej    :  <td align="center" valign="middle">
 ej    :        <b style="font-family: Arial; font-size: 220pt; line-height: 200pt; font-weight: bold;">22</b>
 ej    :  </td>
 ej    : </tr>
 ej    :</table>
 ej    :<img src="stat.asp" width="1" height="1">
 ej    :</body>
hej    :</html> 

