#!/usr/local/bin/perl -w

my $COUNTERFILE ="./config/counters";

unless(dbmopen(%TEST,$COUNTERFILE,0777))
{
    print "checkEnvironment: Cannot open DBM $COUNTERFILE";
}

print "before: EmptyFileCntr: $TEST{'emptyfile'}, LoginFailCntr: $TEST{'loginfail'},InvalidXML:$TEST{'validfail'},Filefail:$TEST{'filefail'},Received:$TEST{'received'}\n";

$TEST{"loginfail"} = 0;
$TEST{"validfail"} = 0;
$TEST{"filefail"} = 0;
$TEST{"received"} =1173255308354;
$TEST{"emptyfile"} = 0;
$TEST{"filecounter"} = 0;

print "after: EmptyFileCntr: $TEST{'emptyfile'}, LoginFailCntr: $TEST{'loginfail'},InvalidXML:$TEST{'validfail'},Filefail:$TEST{'filefail'},Received:$TEST{'received'}\n";

unless(dbmclose(%TEST))
{
   print "checkEnvironment: Cannot close DBM $COUNTERFILE";
}
