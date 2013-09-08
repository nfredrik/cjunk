#!/usr/bin/perl -w

use Switch


$s = 1;
$var = 0x58;


$temp = $var & $s;

for ($i= 0; $i < 8; $i++) {



  switch ($temp) {

  case 0x01 { print "bit 0\n"}
  case 0x02 { print "bit 1\n"}
  case 0x04 { print "bit 2\n"}
  case 0x08 { print "bit 3\n"}
  case 0x10 { print "bit 4\n"}
  case 0x20 { print "bit 5\n"}
  case 0x40 { print "bit 6\n"}
  case 0x80 { print "bit 7\n"}
  else { }
}
  $s = $s << 1;
  $temp = $var &  $s;

#  print "temp  $temp \n";


}
