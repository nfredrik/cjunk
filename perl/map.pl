#!/bin/perl -w

@nums = (0x65,66,67,68);
@chars = map(chr, @nums);

$foo = pack("ccxxcc",65,66,67,68);


$foo1 = pack("cccci",0x21,0x3,0x5,0x1234); # marker, protocol, cmd, option, length
