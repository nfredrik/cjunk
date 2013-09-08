#!/usr/local/bin/perl

# Order to show components:
@order = (
	"FTE.1.1","FTE.1.2","FTE.1.3","FTE.1.4","FTE.1.5","FTE.1.6","FTE.1.7","FTE.1.8","FTE.1.9","FTE.1.10"," ",
	"CME.1.1","CME.1.2","CME.1.3","CME.1.4","CME.1.5","CME.1.6","CME.1.7","CME.1.8","CME.1.9","CME.1.10"," ",
	"LDR.1","DLDR.1"," ",
	"CME.2.90","FTE.2.90","LDR.2","DLDR.2", "BCOL.2.90", " ",
	"AMGR.NERIA", "AMGRW.NERIA", "AMGRW.NIKODEMUS", "AMGRW.NIMROD", "AMGRW.SALOMO"," ",
	"CMD", "CMGR", "CMGRW", "CNSW", "LOGW", "DISC.SALOMO"," "
	);


open(INFILE,"/home2/datachannel/log/proviso.log");
$code=1;
system "/bin/clear";

while (1)
{
	# Läs in så mycket vi får, vänta om vi inte får något
	$code=sysread INFILE,$text,50000;
	if($code == 0)
	{
		sleep 1 ;
	}
	else
	{ 
		@rader = split (/\n/,$text);
		foreach (@rader)
		{
			$num_lines++;
			chomp;
			/(.*?) UTC	(.*?)-\d+	+(.)\	(.*?)	(.*)/;
			$item = $2;
			$time{$item} = $1;
			$action{$item} = "$3 $4";
			$text{$item} = $5;
			$seen{$item}++;
		}

		#update order with newly found items
		%tmp_items = %seen;
		foreach $item (@order)
		{
			delete $tmp_item{$item}
		}
		foreach $item (keys %tmp_item)
		{
			push @order,$item;
		}

		unless ($code == 50000)
		{

			system "/bin/clear";
			$-=0;

			foreach $item (@order)
			{
				write;
			}
		}
	}
}

format STDOUT = 
@<<<<<<<<<<<<<< @<<<< @<<<<<<<<<<<<<<<<<< @<<<<<<<<<<<<<<<<< @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$item,    $seen{$item},$time{$item},      $action{$item},    $text{$item}
.

__END__
2007.08.05-18.44.03 UTC  BCOL.2.90-29167  3  FM_BEGIN_DISK_USAGE  calculating disk usage
2007.08.05-18.44.03 UTC  BCOL.2.90-29167  3  FM_END_DISK_USAGE  finished calculating disk usage (bytes: 699642880  ms: 86)
2007.08.05-18.44.08 UTC  LDR.2-29507  I  SQLLDRPERF  Loaded 79380 NRAW rows in 1 seconds; rate is 79380.0 rec/sec
2007.08.05-18.44.09 UTC  LDR.2-29507  I  POLLSTART  /opt/datachannel/CME.2.90/output
2007.08.05-18.44.10 UTC  LDR.2-29507  1  EXSTATUSCHANGE  Acquiring input files
2007.08.05-18.44.10 UTC  LDR.2-29507  I  ACQUIREINPUTFILES  Found 0 input files
2007.08.05-18.44.10 UTC  LDR.2-29507  1  EXSTATUSCHANGE  Waiting for poll interval (2007.08.03-19.00.00)
2007.08.05-18.44.13 UTC  FTE.1.6-9750  I  POLLSTART  Starting to poll remote dir AMOS:/opt/dataload/SNMP.1.6/output/ for 100 files every 60 seconds.
2007.08.05-18.44.14 UTC  FTE.1.4-9738  I  XFER  Transferred LABAN:/opt/dataload/SNMP.1.4/output/2007.07.31-09/05.00.004.All-meS.NRAW.BOF into /home2/datachannel/FTE.1.4/do/2007.07.31-09/4.05.00.004.All-meS.NRAW.BOF (bytes: 1032080  ms: 13444)
2007.08.05-18.44.16 UTC  CME.2.90-29505  1  EXSTATUSCHANGE  Waiting 60 seconds for data for beyond2007.08.03-19.00.00


