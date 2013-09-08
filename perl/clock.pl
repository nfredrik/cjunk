#!/usr/bin/perl -w
use Tk;

$window = MainWindow->new;
$csize=300;

# Create the clock face

$upper = $window->Frame();
$upper -> pack;
$progress = $upper -> Canvas(-width =>$csize, -height => $csize,
                -background => "green");
$progress->createOval(0,0,$csize,$csize,-fill => "pink");
$progress -> pack;
$run = $upper->Button(-text => "Run", -command => \&startclock);
$run -> pack;

# Add tic marks for hours and minutes, and labels

for ($h=1; $h<=12; $h++) {
   ($x1,$y1) = hand::gettip(0,12,$h,$csize,1.0);
   ($x2,$y2) = hand::gettip(0,12,$h,$csize,0.85);
   $progress->createLine($x1,$y1,$x2,$y2,-fill => "black");
   ($x3,$y3) = hand::gettip(0,12,$h,$csize,0.75);
   $progress->createText($x3,$y3,-text => $h);
   if ($h%3 == 0) {
        ($x1,$y1) = hand::gettip(0,12,$h,$csize,0.3);
           ($x2,$y2) = hand::gettip(0,12,$h,$csize,0.55);
           $progress->createLine($x1,$y1,$x2,$y2,-fill => "black");
        }
   }
for ($h=0; $h<60; $h++) {
   ($x1,$y1) = hand::gettip(0,60,$h,$csize,1.0);
   ($x2,$y2) = hand::gettip(0,60,$h,$csize,0.95);
   $progress->createLine($x1,$y1,$x2,$y2,-fill => "black");
   }

# Create the hands and initially position

@now = gmtime();
$hour = new hand(-start => 0, -end => 12, -current => $now[2]%12 + $now[1]/60.,
        -colour => "black", -length => 0.65, -size => $csize);
$min = new hand(-start => 0, -end => 60,-current => $now[1],
        -colour => "red",-length => 0.85, -size => $csize);
$sec = new hand(-start => 0,-end => 60,-current => $now[0],
        -colour => "black",-length => 0.95, -size => $csize);
startclock();

MainLoop;

# Start and stop clock

sub startclock {
   $step++;
   if ($step%2) {
        $run->configure(-text => "Stop");
   } else {
        $run->configure(-text => "Run");
   }
        runclock();
   }

# Advance clock.  If running, set up an event so that it will
# be advanced again in a further second

sub runclock {
   @now = gmtime();
   $hour -> setcurrent($now[2]%12 + $now[1]/60.0);
   $min -> setcurrent($now[1]);
   $sec -> setcurrent($now[0]);

   $tpiece = gmtime();
   $tpiece =~ s/(\S+:\S+)//;
   $hpiece = $1;
   $tpiece =~ s/\s+/ /g;
   ($x3,$y3) = hand::gettip(0,12,0,$csize,0.1);
   $odate and $progress->delete($odate);
   $odate = $progress->createText($x3,$y3,-text => $tpiece);
   ($x3,$y3) = hand::gettip(0,12,6,$csize,0.1);
   $ohour and $progress->delete($ohour);
   $ohour = $progress->createText($x3,$y3,-text => $hpiece);

   $progress->update;
   if ($step%2) {
        $progress->after(1000,\&runclock);
   }
}

# Encapsulate the calculations to keep details of the movements
# of the end of each hand in a single part of the code.

package hand;

# Initial hand positioning

sub new {
        my ($class,%info) = @_;
        $count++;
        $handname = "hand$count";
        ($cx,$cy) = gettip($info{"-start"},$info{"-end"},
                        $info{"-current"},$info{"-size"},
                $info{"-length"});
        $info{cx} = $cx;
        $info{cy} = $cy;
        $info{handname} = $handname;
        $main::progress->createOval($cx-10,$cy-10,$cx+10,$cy+10,
                        -fill => $info{"-colour"},
                -tag => $handname);
        bless \%info,$class;
        }

# Method to reposition the end of a hand

sub setcurrent {
        my ($info,$newval) = @_;
        ($cx,$cy) = gettip($$info{"-start"},$$info{"-end"},$newval,$$info{"-size"},
                $$info{"-length"});
        $chx = $cx - $$info{cx};
        $chy = $cy - $$info{cy};
        $$info{cx} = $cx;
        $$info{cy} = $cy;
        $$info{-current} = $newval;
        $main::progress->move($$info{handname},$chx,$chy);
        }

# Utility method - calculate hand end based on hour or minute and
# also on arm length

sub gettip {
        my ($from, $to, $current, $size, $length) = @_;
        $angle = 90.0 - 360 * (1.0 * $current / ($to - $from));
        $newx = cos($angle / 57.295779) * $size * $length / 2;
        $newy = -sin($angle / 57.295779) * $size * $length / 2;
        return ($newx + $size/2 ,$newy + $size / 2);
        }
 
