#!/usr/local/bin/perl

#foreach (`cat /home2/datachannel/log/*_proviso.log /home2/datachannel/log/proviso.log`)
foreach (`cat /home2/datachannel/log/proviso.log`)
{
	next unless /(\d{4})\.(\d{2})\.(\d{2})-(\d{2}).*?CME.1.$ARGV[0]-.*CURRENT_HOUR	Processing hour (\d{4})\.(\d{2})\.(\d{2})-(\d{2})/;
	($lyear,$lmonth,$lday,$lhour) = ($1,$2,$3,$4);
	($pyear,$pmonth,$pday,$phour) = ($5,$6,$7,$8);
	$log = $lhour + $lday*24 + $lmonth * 31 *24 ;
	$proc = $phour + $pday*24 + $pmonth * 31 *24 ;
	$lag = $log - $proc;
	print "$lyear-$lmonth-$lday-$lhour	$lag\n";
}

__END__
2007.08.10-07.54.34 UTC CME.1.10-25580  I       CURRENT_HOUR    Processing hour 2007.08.07-09.00.00
neria:/home2/datachannel/log % cat *_proviso.log proviso.log | grep CURRENT_HOUR | grep CME.1.10-

