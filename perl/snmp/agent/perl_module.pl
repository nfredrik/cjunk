#!/usr/bin/perl
#
# This is an example of perl module support for the net-snmp agent.
#
# To load this into a running agent with embedded perl support turned
# on, simply put the following line (without the leading # mark) your
# snmpd.conf file:
#
#   perl do "/path/to/perl_module.pl";
#

use constant CHANNEL_ON        => 1;
use constant CHANNEL_OFF       => 0;
use constant CHANNEL_VOLTAGE   => 0x40000000;
use constant DEVIATION         => 1;

my $regat = '.1.3.6.1.4.1.19947';


our $m_power     = 0;
@power_slot1 = ( 0, 0, 0, 0, 0, 0, 0, 0 );
@power_slot2 = ( 0, 0, 0, 0, 0, 0, 0, 0 );
@power_slot3 = ( 0, 0, 0, 0, 0, 0, 0, 0 );
@power_slot4 = ( 0, 0, 0);


@voltage_slot1 = ( 0, 0, 0, 0, 0, 0, 0, 0 );
@voltage_slot2 = ( 0, 0, 0, 0, 0, 0, 0, 0 );
@voltage_slot3 = ( 0, 0, 0, 0, 0, 0, 0, 0 );
@voltage_slot4 = ( 0, 0, 0);

my $voltage_base = $regat . ".1.3.1.2.10";

BEGIN {
    print STDERR "starting perl_module.pl\n";
}

use SNMP;
use NetSNMP::OID (':all');
use NetSNMP::agent (':all');
use NetSNMP::ASN (':all');

print STDERR "perl_module.pl loaded ok\n";

# set to 1 to get extra debugging information
$debugging = 1;

# if we're not embedded, this will get auto-set below to 1
$subagent = 0;

# Add path to MIBS
#SNMP::addMibDirs('/usr/share/snmp/mibs');

# load MIB!
#SNMP::loadModules('WIENER-CRATE-MIB');
#SNMP::InitMib();

# where we are going to hook onto
my $regoid = new NetSNMP::OID('.1.3.6.1.4.1.19947');
print STDERR "registering at ",$regoid,"\n" if ($debugging);

# If we're not running embedded within the agent, then try to start
# our own subagent instead.
if (!$agent) {
    $agent = new NetSNMP::agent('Name' => "test", # reads test.conf
#                                'Ports' => "tcp:705",
#                                'Ports' => "tcp:192.168.2.18:705",
				'AgentX' => 1   );   # make us a subagent
    $subagent = 1;
    print STDERR "started us as a subagent ($agent)\n"
}

# we register ourselves with the master agent we're embedded in.  The
# global $agent variable is how we do this:


$agent->register("test",$regoid, \&my_snmp_handler);

if ($subagent) {
    # We need to perform a loop here waiting for snmp requests.  We
    # aren't doing anything else here, but we could.
    $SIG{'INT'} = \&shut_it_down;
    $SIG{'QUIT'} = \&shut_it_down;
    $running = 1;
    while($running) {
	$agent->agent_check_and_process(1);  # 1 = block
	print STDERR "mainloop excercised\n" if ($debugging);
    }
    $agent->shutdown();
}


#print STDERR "going to main_loop\n" if ($debugging);


#$agent->agent_check_and_process(1);  # 1 = block

######################################################################
# define a subroutine to actually handle the incoming requests to our
# part of the OID tree.  This subroutine will get called for all
# requests within the OID space under the registration oid made above.
sub my_snmp_handler {
    my ($handler, $registration_info, $request_info, $requests) = @_;
    my $request;



    print STDERR "refs: ",join(", ", ref($handler), ref($registration_info), 
			       ref($request_info), ref($requests)),"\n";

    print STDERR "processing a request of type " . $request_info->getMode() . "\n"
	if ($debugging);


    for($request = $requests; $request; $request = $request->next()) {
      my $oid = $request->getOID();
      print STDERR "  processing request of $oid\n";


######################################################################
#
#
#       GET
#
#
######################################################################

      if ($request_info->getMode() == MODE_GET) {
	# if the requested oid is equals to ours, then return the data

        #
        # MainSwitch: on or off?
        #
        #
  	my $main_power = new NetSNMP::OID($regat . ".1.1.1.0");
	if ($oid == new NetSNMP::OID($main_power)) {
	    $request->setValue(ASN_INTEGER,$m_power);
	}

        #  Switches: On or Off ?
        #
        my $ch;
	my $power = new NetSNMP::OID($regat . ".1.3.2.1.9");

        # Slot 1
        for($ch = 1; $ch<9; $ch++) {

   	   my $channel = new NetSNMP::OID($power .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
#	     $request->setValue(ASN_INTEGER,CHANNEL_ON);
	     $request->setValue(ASN_INTEGER,$power_slot1[$ch-1]);
	   }
	 }

        # Slot 2
        for($ch = 101; $ch<109; $ch++) {

   	   my $channel = new NetSNMP::OID($power .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
	     $request->setValue(ASN_INTEGER,$power_slot2[$ch-101]);
	   }
	 }

        # Slot 3
        for($ch = 201; $ch<209; $ch++) {

   	   my $channel = new NetSNMP::OID($power .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
	     $request->setValue(ASN_INTEGER,$power_slot3[$ch-201]);
	   }
	 }

        # Slot 4
        for($ch = 301; $ch<304; $ch++) {

   	   my $channel = new NetSNMP::OID($power .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
	     $request->setValue(ASN_INTEGER,$power_slot4[$ch-301]);
	   }
	 }

        #
        #  Get Setpoint Channel values: 
        #
	my $voltages = new NetSNMP::OID($regat . ".1.3.2.1.10");


        # Slot 1
        for($ch = 1; $ch<9; $ch++) {

   	   my $channel = new NetSNMP::OID($voltages .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
             if($power_slot1[$ch-1]) {
	       $request->setValue(ASN_INTEGER,$voltage_slot1[$ch-1]);
             }
             else {
	       $request->setError($request_info,SNMP_ERR_GENERR);
	   
	     }
	   }
	 }


        # Slot 2
        for($ch = 101; $ch<109; $ch++) {

   	   my $channel = new NetSNMP::OID($voltages .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
             if($power_slot2[$ch-101]){ 
	       $request->setValue(ASN_INTEGER,$voltage_slot2[$ch-101]);
	     }
             else {
	       $request->setError($request_info,SNMP_ERR_GENERR);
	     }
	   }
	 }

        # Slot 3
        for($ch = 201; $ch<209; $ch++) {

   	   my $channel = new NetSNMP::OID($voltages .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
             if($power_slot3[$ch-201]) {
	       $request->setValue(ASN_INTEGER,$voltage_slot3[$ch-201]);
             }
	     else {
	       $request->setError($request_info,SNMP_ERR_GENERR);
	     }
	   }
	 }

        # Slot 4
        for($ch = 301; $ch<304; $ch++) {

   	   my $channel = new NetSNMP::OID($voltages .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
             if($power_slot4[$ch-301]) {
	       $request->setValue(ASN_INTEGER,$voltage_slot4[$ch-301]);
	     }
	     else {
	       $request->setError($request_info,SNMP_ERR_GENERR);
	     }
	   }
	 }


        #
        #  Actual Channel values: 
        #

	my $avoltages = new NetSNMP::OID($regat . ".1.3.2.1.5");


        # Slot 1
        for($ch = 1; $ch<9; $ch++) {

   	   my $channel = new NetSNMP::OID($avoltages .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
             if($power_slot1[$ch-1]) {
	       $request->setValue(ASN_INTEGER,$voltage_slot1[$ch-1] + DEVIATION);
	     }
	     else {
	       $request->setError($request_info,SNMP_ERR_GENERR);
	     }
	   }
	 }


        # Slot 2
        for($ch = 101; $ch<109; $ch++) {

   	   my $channel = new NetSNMP::OID($avoltages .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
             if($power_slot2[$ch-101]) {
	       $request->setValue(ASN_INTEGER,$voltage_slot2[$ch-101] + DEVIATION);
#
# Generate a problem i.e setpoint and actual value differs too much
#
#	       $request->setValue(ASN_INTEGER,900);
	     }
	     else {
	       $request->setError($request_info,SNMP_ERR_GENERR);
	     }
	   }
	 }

        # Slot 3
        for($ch = 201; $ch<209; $ch++) {

   	   my $channel = new NetSNMP::OID($avoltages .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
             if($power_slot3[$ch-201]) {
	       $request->setValue(ASN_INTEGER,$voltage_slot3[$ch-201] + DEVIATION);
	     }
	     else {
	       $request->setError($request_info,SNMP_ERR_GENERR);
	     }
	   }
	 }

        # Slot 4
        for($ch = 301; $ch<304; $ch++) {

   	   my $channel = new NetSNMP::OID($avoltages .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
             if($power_slot4[$ch-301]) {
	       $request->setValue(ASN_INTEGER,$voltage_slot4[$ch-301] + DEVIATION);
	     }
	     else {
	       $request->setError($request_info,SNMP_ERR_GENERR);
	     }
	   }
	 }



######################################################################
#
#
#       SET
#
#
######################################################################


      } elsif ($request_info->getMode() == MODE_SET_ACTION) {



  	my $main_power = new NetSNMP::OID($regat . ".1.1.1.0");
	if ($oid == new NetSNMP::OID($main_power)) {
        $value = $request->getValue();

        if(!($value)) {

	  @power_slot1 = ( 0, 0, 0, 0, 0, 0, 0, 0 );
	  @power_slot2 = ( 0, 0, 0, 0, 0, 0, 0, 0 );
	  @power_slot3 = ( 0, 0, 0, 0, 0, 0, 0, 0 );
	  @power_slot4 = ( 0, 0, 0);

	  @voltage_slot1 = ( 0, 0, 0, 0, 0, 0, 0, 0 );
	  @voltage_slot2 = ( 0, 0, 0, 0, 0, 0, 0, 0 );
	  @voltage_slot3 = ( 0, 0, 0, 0, 0, 0, 0, 0 );
	  @voltage_slot4 = ( 0, 0, 0);
	} 
        # Set main switch
        $m_power = $value;
      }
        # Set  switches 


        my $ch;
	my $power = new NetSNMP::OID($regat . ".1.3.2.1.9");

        # Slot 1
        for($ch = 1; $ch<9; $ch++) {

   	   my $channel = new NetSNMP::OID($power .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
              $value = $request->getValue();
              $power_slot1[$ch-1] = $value;
              if(!($value)) {$voltage_slot1[$ch-1] = 0;}
	      print STDERR "   -> Channel $ch Got on or off?: $value\n";
	   }
	 }

        # Slot 2
        for($ch = 101; $ch<109; $ch++) {

   	   my $channel = new NetSNMP::OID($power .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
              $value = $request->getValue();
              $power_slot2[$ch-101] = $value;
              if(!($value)) {$voltage_slot2[$ch-101] = 0;}
	      print STDERR "   -> Channel $ch Got on or off?: $value\n";
	   }
	 }

        # Slot 3
        for($ch = 201; $ch<209; $ch++) {

   	   my $channel = new NetSNMP::OID($power .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
              $value = $request->getValue();
              $power_slot3[$ch-201] = $value;
              if(!($value)) {$voltage_slot3[$ch-201] = 0;}
	      print STDERR "   -> Channel $ch Got on or off?: $value\n";
	   }
	 }

        # Slot 4
        for($ch = 301; $ch<304; $ch++) {

   	   my $channel = new NetSNMP::OID($power .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
              $value = $request->getValue();
              $power_slot4[$ch-301] = $value;
              if(!($value)) {$voltage_slot4[$ch-301] = 0;}
	      print STDERR "   -> Channel $ch Got on or off?: $value\n";
	   }
	 }
        
        #
        #  Set "Setpoint Channel values:"
        #


        my $ch;
	my $svoltages = new NetSNMP::OID($regat . ".1.3.2.1.10");


        # Slot 1
        for($ch = 1; $ch<9; $ch++) {

   	   my $channel = new NetSNMP::OID($svoltages .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
             if($power_slot1[$ch-1])  {
	       $value = $request->getValue();
	       $voltage_slot1[$ch-1] = $value;
	     }
	     else {
	       $request->setError($request_info,SNMP_ERR_GENERR);
	     }
	   }
	 }

        # Slot 2
        for($ch = 101; $ch<109; $ch++) {

   	   my $channel = new NetSNMP::OID($svoltages .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
             if($power_slot2[$ch-101])  {
	       $value = $request->getValue();
	       $voltage_slot2[$ch-101] = $value;
	     }
	     else {
	       $request->setError($request_info,SNMP_ERR_GENERR);
	     }
	   }
	 }

        # Slot 3
        for($ch = 201; $ch<209; $ch++) {

   	   my $channel = new NetSNMP::OID($svoltages .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
             if($power_slot3[$ch-201])  {
	       $value = $request->getValue();
	       $voltage_slot3[$ch-201] = $value;
	     }
	     else {
	       $request->setError($request_info,SNMP_ERR_GENERR);
	     }
	   }
	 }

        # Slot 4
        for($ch = 301; $ch<304; $ch++) {

   	   my $channel = new NetSNMP::OID($svoltages .'.' . $ch);
	   if ($oid == new NetSNMP::OID($channel)) {
             if($power_slot4[$ch-301])  {
	       $value = $request->getValue();
	       $voltage_slot4[$ch-301] = $value;
	     }
	     else {
	       $request->setError($request_info,SNMP_ERR_GENERR);
	     }
	   }
	 }


######################################################################
#
#
#       GET_NEXT
#
#
######################################################################

       } elsif ($request_info->getMode() == MODE_GETNEXT) {
	# if the requested oid is lower than ours, then return ours
	if ($oid < new NetSNMP::OID($regat . ".1.2.1")) {
	  print STDERR "   $regat.1.2.1 -> hello world\n" if ($debugging);
	  $request->setOID($regat . ".1.2.1");
	  $request->setValue(ASN_OCTET_STR, "hello world2");
	}
      }
    }

    print STDERR "  finished processing\n"
	if ($debugging);
}

sub shut_it_down {
  $running = 0;
  print STDERR "shutting down\n" if ($debugging);
}


__END__


	if ($oid == new NetSNMP::OID($regat . ".1.3.2.1.9.12")) {
	  print STDERR "   -> hello world\n" if ($debugging);
	  $request->setValue(ASN_INTEGER,1);
	}
	if ($oid == new NetSNMP::OID($regat . ".1.3.2.1.9.105")) {
	  print STDERR "   -> hello world\n" if ($debugging);
	  $request->setValue(ASN_INTEGER,1);
	}

        my $ch = 16;
	my $power = new NetSNMP::OID($regat . ".1.3.2.1.9");
#	my $channel = new NetSNMP::OID($power .".15");
	my $channel = new NetSNMP::OID($power .'.' . $ch);
        print STDERR "Here we are: " . $channel . "\n";
	if ($oid == new NetSNMP::OID($channel)) {
#	if ($oid == new NetSNMP::OID($regat . ".1.3.2.1.9.15")) {   this one works
#	if ($oid == new NetSNMP::OID($regat . ".1.3.2.1.9" . ".15")) {
#	if ($oid == new NetSNMP::OID($regat . $power . ".15")) {
	  print STDERR "   -> hello fredde\n" if ($debugging);
	  $request->setValue(ASN_INTEGER,1);
	}





	if ($oid == new NetSNMP::OID($regat . ".1.3.2.1.10.105")) {
	  print STDERR "   -> get voltage, type:", ASN_OCTET_STR, "\n" if ($debugging);
#	  $request->setOID($regat . ".1.3.2.1.10.105");
#	  $request->setValue(ASN_OPAQUE,444);
#	  $request->setValue(ASN_INTEGER,0x44445566);
	  $request->setValue(ASN_INTEGER,0x40000000);
#	  $request->setValue(ASN_OCTET_STR,"444");
#	  $request->setValue(ASN_BIT_STR,"444");
#	  $request->setValue(ASN_OPAQUE,"44079f780442f6000");

	}


@power_slot2 = (CHANNEL_OFF, CHANNEL_OFF, CHANNEL_OFF, CHANNEL_OFF, CHANNEL_OFF CHANNEL_OFF, CHANNEL_OFF, CHANNEL_OFF);
@power_slot3 = (CHANNEL_OFF, CHANNEL_OFF, CHANNEL_OFF, CHANNEL_OFF, CHANNEL_OFF CHANNEL_OFF, CHANNEL_OFF, CHANNEL_OFF);
@power_slot4 = (CHANNEL_OFF, CHANNEL_OFF, CHANNEL_OFF, CHANNEL_OFF, CHANNEL_OFF CHANNEL_OFF, CHANNEL_OFF, CHANNEL_OFF);



@voltage_slot1 = ( 150, 400, 500, 600, 900, 1000, 1100, 1400);
@voltage_slot2 = ( 1500, 1600, 1900, 2000, 2100, 2400, 2500, 2600);
@voltage_slot3 = ( 2900, 3000, 3100, 3400, 3500, 3600, 3900, 4000);
@voltage_slot4 = ( 4100, 4400, 4500 );
