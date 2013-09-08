#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
#                           P V C O M M O N . P M
#
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# FileName      : PVCommon.pm
# Author        : ACOS
# Date          : 22/02/2004
# Subjet        :
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Copyright (c) 2000-2004 QUALLABY Corporation. All rights reserved.
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package PVCommon;

#===============================================================================
# Requirements
#===============================================================================
require Exporter;

use Sys::Hostname;
use Net::hostent;
use Net::Ping;
use Net::Ping::External qw(ping);
use IO::Socket::INET;


use strict;

#===============================================================================
# Global variables
#===============================================================================

our($VERSION) = "1.00";
our(@ISA)=("Exporter");
our(@EXPORT)=qw(
  FlattenTable
  RemoveQuotes
  UTF8toASCII
  Txt2XMLEntity
  GetHostName
  GetUserName
  SameHost
  Ping
  strtrim
  IsInTable
  GetDataMartHome
  GetDataLoadHome
  GetDataMartUser
  GetDataLoadUser
  GetDataLoadConfig
  GetDataLoadEnv
  RemoveDataLoadDataDir
  RegisterDataLoad
  StartDataLoad
  StopDataLoad
  Decrypt
  CheckResourceManager
  PrintObjectList
);

our($DATAMART_HOME);
our($DATALOAD_HOME);

#-------------------------------------------------------------------------------
# FlattenTable
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
# Input        :
# Return       :
#-------------------------------------------------------------------------------

sub FlattenTable
{
  my(@table)=@_;
  my(@table2);
  foreach my $item (@table)
  {
    push(@table2, split(/,|;|:/, $item));
  }
  return @table2;
}

#-------------------------------------------------------------------------------
# RemoveQuotes
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
# Input        :
# Return       :
#-------------------------------------------------------------------------------

sub RemoveQuotes($)
{
  my($str)=shift || return undef;
  $str =~ s/^\"(.*)\"$/$1/;
  return $str;
}

#-------------------------------------------------------------------------------
# UTF8toASCII
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
# Input        :
# Return       :
#-------------------------------------------------------------------------------

sub UTF8toASCII($)
{
  my($str)=shift || return undef;

  $str =~ s/[\ª\©\®\ø\Ï\Î]/A/g;
  $str =~ s/[\ˇ]/B/g;
  $str =~ s/[\•\ò\÷\≤\‰\ﬁ]/a/g;
  $str =~ s/[\“]/C/g;
  $str =~ s/[\Á\∆]/c/g;
  $str =~ s/[\¿\∂\ã\ˆ]/E/g;
  $str =~ s/[\Ë\„\Í\‚]/e/g;
  $str =~ s/[\”\Õ\≥\¥]/I/g;
  $str =~ s/[\≠\¨\Ì\È]/i/g;
  $str =~ s/[\€]/N/g;
  $str =~ s/[\–]/n/g;
  $str =~ s/[\ö\‹\õ\¢\ƒ\Û]/O/g;
  $str =~ s/[\∏\À\Ù\≈\ó\π]/o/g;
  $str =~ s/[\˝\è\¸\]/U/g;
  $str =~ s/[\ﬂ\¯\£\˚]/u/g;
  $str =~ s/[\ÿ]/Y/g;
  $str =~ s/[\]/y/g;
#  $str =~ s/[«]/ae/g;
  $str =~ s/[^\w\s\\\/<\>\(\)\[\]\{\}\|\.\;\:\,\!\?\&\-\+\*\=\$\%\~\@\^\"\'\`\#]/?/g;

  return $str;
}

#-------------------------------------------------------------------------------
# Txt2XMLEntity
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
# Input        :
# Return       :
#-------------------------------------------------------------------------------

sub Txt2XMLEntity($)
{
  my($str)=shift || return undef;

  $str =~ s/\<(.+)\>(.*)\&(.*)\<\/\1\>/\<$1\>$2\&amp\;$3\<\/$1\>/g;
  $str =~ s/\<(.+)\>(.*)\<(.*)\<\/\1\>/\<$1\>$2\&lt\;$3\<\/$1\>/g;
  $str =~ s/\<(.+)\>(.*)\>(.*)\<\/\1\>/\<$1\>$2\&gt\;$3\<\/$1\>/g;
  $str =~ s/\<(.+)\>(.*)\"(.*)\<\/\1\>/\<$1\>$2\&quot\;$3\<\/$1\>/g;
  $str =~ s/\<(.+)\>(.*)\'(.*)\<\/\1\>/\<$1\>$2\&apos\;$3\<\/$1\>/g;

  return $str;
}

#-------------------------------------------------------------------------------
# PVCommon::GetHostName
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
# Input        : $str
# Return       : $str
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub GetHostName
{
  my($localhost)=hostname();

  if (not $localhost)
  {
    $localhost=`hostname 2>/dev/null`;
    chomp($localhost);
  }
  if (not $localhost)
  {
    $localhost=`uname -n 2>/dev/null`;
    chomp($localhost);
  }
  if (not $localhost)
  {
    $localhost="localhost";
  }
  return uc($localhost);
}

#-------------------------------------------------------------------------------
# PVCommon::GetUserName
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
# Input        : $str
# Return       : $str
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub GetUserName
{
  my($logname)= $ENV{ USER }      ||
                $ENV{ LOGNAME }   ||
                getlogin()        ||
                (getpwuid($<))[0] ||
                "Unknown uid $<";
  chomp($logname);
  return $logname;
}

#-------------------------------------------------------------------------------
# PVCommon::SameHost
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#   Is it the same
# Input        :
# Return       :
#   1 = Success, 0= Fail
#-------------------------------------------------------------------------------

sub SameHost
{
  my($name1)=shift;
  my($name2)=shift;
  my($h1)=gethost($name1);
  my($h2)=gethost($name2);

  if ((not defined $h1) or (not defined $h2))
  {
    return 0;
  }
  if (($h1->name eq $h2->name) or
      (inet_ntoa($h1->addr) eq inet_ntoa($h2->addr)))
  {
    return 1;
  }
  return 0;
}

#-------------------------------------------------------------------------------
# PVCommon::Ping
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
# Input        : None
# Return       : None
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub Ping
{
  my($host)=shift;
  my($res)=0;
  my($try)=0;

  while ($try < 2)
  {
    $try++;
    if (ping(hostname => $host, timeout => 2*($try+1)))
    {
      $res=1;
      last;
    }
  }
  return $res;
}

#-------------------------------------------------------------------------------
# PVCommon::strtrim
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Remove the blanks characters at the begining and at the end of
#                the given string.
# Input        : $str
# Return       : $str
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub strtrim($)
{
  my($str)=$_[0];

  return undef if (not defined $str);
  $str =~ s/(^\s+|\s+$)//g;
  return $str;
}

#-------------------------------------------------------------------------------
# PVCommon::IsInTable
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Search in the given indexed table a value equal to the given
#                keyword.
# Input        : ($table, $keyword)
# Return       : (1=Succeded, 0=Failed)
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub IsInTable($$)
{
  my($table)=$_[0];
  my($keyword)=$_[1];

  return 0 if (not defined $keyword);
  foreach (@$table)
  {
    if ($_ eq $keyword)
    {
      return 1;
    }
  }
  return 0;
}

#-------------------------------------------------------------------------------
# PVCommon::GetDataMartHome
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Try to infer the DataMart Home directory
# Input        : None
# Return       : $home or undef if error
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub GetDataMartHome
{
  my($home)=undef;

  if (exists($ENV{ 'PVMHOME' }))
  {
    $DATAMART_HOME=$ENV{ 'PVMHOME' };
    return $ENV{ 'PVMHOME' };
  }

# Hmmm, absolut v‰g???
  open(FILE, "/etc/pisrc3113ProvisoInfoServer.sh") || return undef;
  while(<FILE>)
  {
    chomp($_);
    next if ($_ =~ m/^(#.*|\s*)$/ );
    my(@line)=split("=", $_);
    if ($line[0] eq "PVMHOME")
    {
      $home=$line[1];
      $ENV{ 'PVMHOME' }=$home;
      $DATAMART_HOME=$home;
      last;
    }
  }
  close(FILE) || return undef;
  return $home;
}

#-------------------------------------------------------------------------------
# PVCommon::GetDataLoadHome
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Try to infer the DataLoad Home directory
# Input        : None
# Return       : $home or undef if error
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub GetDataLoadHome
{
  my($home)=undef;

  if (exists($ENV{ 'PVMHOME' }))
  {
    $DATALOAD_HOME=$ENV{ 'PVMHOME' };
    return $ENV{ 'PVMHOME' };
  }

  open(FILE, "/etc/netpvmdrc.sh") || return undef;
  while(<FILE>)
  {
    chomp($_);
    next if ($_ =~ m/^(#.*|\s*)$/ );
    my(@line)=split("=", $_);
    if ($line[0] eq "PVMHOME")
    {
      $home=$line[1];
      $ENV{ 'PVMHOME' }=$home;
      $DATALOAD_HOME=$home;
      last;
    }
  }
  close(FILE) || return undef;
  return $home;
}

#-------------------------------------------------------------------------------
# PVCommon::GetDataMartUser
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Try to infer the DataMart user
# Input        : None
# Return       : $user or undef if error
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub GetDataMartUser
{
  my($user);
  my($uid);
  GetDataMartHome if not defined $DATAMART_HOME;

  $uid=(stat("$DATAMART_HOME/dataMart.env"))[4];
  open(FILE, "/etc/passwd");
  while(<FILE>)
  {
    chomp($_);
    my(@line)=split(/:/,$_);
    if ((defined $line[2]) && ($line[2] == $uid))
    {
      $user=$line[0];
      last;
    }
  }
  close(FILE);
  return $user;
}

#-------------------------------------------------------------------------------
# PVCommon::GetDataLoadUser
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Try to infer the DataLoad user
# Input        : None
# Return       : $user or undef if error
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub GetDataLoadUser
{
  my($user);
  my($uid);
  GetDataLoadHome if not defined $DATALOAD_HOME;

  $uid=(stat("$DATALOAD_HOME/dataLoad.env"))[4];
  open(FILE, "/etc/passwd");
  while(<FILE>)
  {
    chomp($_);
    my(@line)=split(/:/,$_);
    if ((defined $line[2]) && ($line[2] == $uid))
    {
      $user=$line[0];
      last;
    }
  }
  close(FILE);
  return $user;
}

#-------------------------------------------------------------------------------
# PVCommon::GetDataLoadEnv
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Load the environement file and modify the current environment.
# Input        : None
# Return       : (1=Succeded, 0=Failed)
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub GetDataLoadEnv
{
  GetDataLoadHome if not defined $DATALOAD_HOME;

  open(FILE, "$DATALOAD_HOME/dataLoad.env") || return 0;
  while(<FILE>)
  {
    chomp($_);
    next if ($_ =~ m/^(#.*|\s*)$/ );
    my(@line)=split(/=/, $_);
    next if ($#line < 1);
    foreach my $i ('PVM_HOME',
                   'PVM_DATABASE',
                   'PVM_DATABASE_NAME',
                   'PVM_DATABASE_SERVER_NAME',
                   'ORACLE_HOME',
                   'ORACLE_SID',
                   'NLS_LANG')
    {
      if (strtrim($line[0]) eq $i)
      {
        $ENV{ $i } = strtrim($line[1]);
      }
    }
  }
  close(FILE) || return 0;
  return 1;
}

#-------------------------------------------------------------------------------
# PVCommon::GetDataLoadConfig
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Load the configuration file
# Input        : None
# Return       : \%Config or undef if error
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub GetDataLoadConfig
{
  my(%Config);
  GetDataLoadHome if not defined $DATALOAD_HOME;

  open(FILE, "$DATALOAD_HOME/conf/database.conf") || return undef;
  while(<FILE>)
  {
    chomp($_);
    next if ($_ =~ m/^(\!\!.*|\s*)$/ );
    my(@line)=split(":", $_);
    next if ($#line < 1);
    if ($line[0] =~ m/^\*.+_[^_]+_[^_]+_user$/)
    {
      $Config{ 'hostname' } = $line[0];
      $Config{ 'hostname' } =~ s/\*(.+)_[^_]+_[^_]+_user/$1/;
      $Config{ 'tnsentry' } = $line[0];
      $Config{ 'tnsentry' } =~ s/\*.+_([^_]+)_[^_]+_user/$1/;
      $Config{ 'dbname'   } = $line[0];
      $Config{ 'dbname'   } =~ s/\*.+_[^_]+_([^_]+)_user/$1/;
      $Config{ 'username' } = strtrim($line[1]);
    }
    if ($line[0] =~ m/^\*.+_[^_]+_[^_]+_pwd$/)
    {
      $Config{ 'password' } = strtrim($line[1]);
    }
  }
  close(FILE) || return undef;
  return \%Config;
}

#-------------------------------------------------------------------------------
# PVCommon::RemoveDataLoadDataDir
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Remove all local statistic data files
# Input        : None
# Return       : (1=Succeded, 0=Failed)
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub RemoveDataLoadDataDir
{
  GetDataLoadHome if not defined $DATALOAD_HOME;

  system("rm -rf $DATALOAD_HOME/SNMP* 1>/dev/null 2>&1");

  return ($? >> 8 == 0)?1:0;
}

#-------------------------------------------------------------------------------
# PVCommon::RegisterDataLoad
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Register the Proviso DataLoad SNMP collector daemon
# Input        : None
# Return       : (1=Succeded, 0=Failed)
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub RegisterDataLoad
{
  my($chnl)=shift;
  my($subchnl)=shift;
  my($logicalhostname)=shift;

  GetDataLoadHome if not defined $DATALOAD_HOME;

  return 0 if (not -f "$DATALOAD_HOME/conf/dl.cfg");
  return 0 if (not -f "$DATALOAD_HOME/dataLoad.env");
  return 0 if (not -x "$DATALOAD_HOME/bin/pvmd");

  system("cp -p $DATALOAD_HOME/dataLoad.env " .
         " $DATALOAD_HOME/dataLoad.env.old " .
         " 2>/dev/null");

  system("cat $DATALOAD_HOME/conf/dl.cfg | ".
         " sed 's/SNMP\.[0-9]*\.[0-9]*\./SNMP\.$chnl\.$subchnl\./g' | " .
         " sed 's/WATCHD\.[0-9]*\.[0-9]*\./WATCHD\.$chnl\.$subchnl\./g' " .
         " > $DATALOAD_HOME/conf/dl.cfg.new && " .
         " mv $DATALOAD_HOME/conf/dl.cfg.new $DATALOAD_HOME/conf/dl.cfg ".
         " 1>/dev/null 2>&1");

  return 0 if ($? >> 8 != 0);

  system("sed 's/PVM_SNMP_COLL_CHANNEL=[0-9]*/PVM_SNMP_COLL_CHANNEL=$chnl/g' " .
         " $DATALOAD_HOME/dataLoad.env > $DATALOAD_HOME/dataLoad.env.new && " .
         " mv $DATALOAD_HOME/dataLoad.env.new $DATALOAD_HOME/dataLoad.env  " .
         " 1>/dev/null 2>&1");

  return 0 if ($? >> 8 != 0);

  system("sed 's/PVM_SNMP_COLL_NUMBER=[0-9]*/PVM_SNMP_COLL_NUMBER=$subchnl/g' " .
         " $DATALOAD_HOME/dataLoad.env > $DATALOAD_HOME/dataLoad.env.new && " .
         " mv $DATALOAD_HOME/dataLoad.env.new $DATALOAD_HOME/dataLoad.env  " .
         " 1>/dev/null 2>&1");

  return 0 if ($? >> 8 != 0);

  system("sed 's/PVMEnvProductName=.*/PVMEnvProductName=DataLoad_$subchnl/g' " .
         " $DATALOAD_HOME/dataLoad.env > $DATALOAD_HOME/dataLoad.env.new && " .
         " mv $DATALOAD_HOME/dataLoad.env.new $DATALOAD_HOME/dataLoad.env  " .
         " 1>/dev/null 2>&1");

  return 0 if ($? >> 8 != 0);

  system(". $DATALOAD_HOME/dataLoad.env && " .
         " $DATALOAD_HOME/bin/pvmd -install " .
         " -c " . $chnl .
         " -l " . $subchnl .
         " -f $DATALOAD_HOME/conf/dl.cfg " .
         " -logicalhostname " . $logicalhostname .
         " >/dev/null 2>&1");

  return 0 if ($? >> 8 != 0);

  return 1;
}

#-------------------------------------------------------------------------------
# PVCommon::StartDataLoad
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Start the Proviso DataLoad SNMP collector daemon
# Input        : None
# Return       : (1=Succeded, 0=Failed)
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub StartDataLoad
{
  GetDataLoadHome if not defined $DATALOAD_HOME;

  my($user)=GetDataLoadUser();
  if (not defined $user)
  {
    return 0;
  }
  system(". $DATALOAD_HOME/dataLoad.env && $DATALOAD_HOME/bin/pvmdmgr start 1>/dev/null 2>&1");

  return ($? >> 8 == 0)?1:0;
}

#-------------------------------------------------------------------------------
# PVCommon::StopDataLoad
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Stop the Proviso DataLoad SNMP collector daemon
# Input        : None
# Return       : (1=Succeded, 0=Failed)
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub StopDataLoad
{
  GetDataLoadHome if not defined $DATALOAD_HOME;

  my($user)=GetDataLoadUser();
  if (not defined $user)
  {
    return 0;
  }
  system(". $DATALOAD_HOME/dataLoad.env && $DATALOAD_HOME/bin/pvmdmgr stop 1>/dev/null 2>&1");

  return ($? >> 8 == 0)?1:0;
}

#-------------------------------------------------------------------------------
# PVCommon::Decrypt
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Decrypt Proviso passwords
# Input        : $in
# Return       : $out
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub Decrypt
{
  my($in)=shift;
  my($crypt1)="amqpslzodkamqpslzodkamqpslzodkderf";
  my($crypt2)="19324785491932478519324785193247850";
  my($out)="";
  my($i1)=0;
  my($i2)=0;
  for(my $i=0 ; $i<length($in) ; $i+=2)
  {
    my($val)=((ord(substr($in,$i))-ord('A'))*16)%255;
    $val+=ord(substr($in,$i+1))-ord('A');
    $out.=chr(($val^ord(substr($crypt2,$i2++))^ord(substr($crypt1,$i1++))));
    $i1=0 if ($i1>=length($crypt1));
    $i2=0 if ($i2>=length($crypt2));
  }
  return $out;
}

#-------------------------------------------------------------------------------
# PVCommon::CheckResourceManager
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Check if the Resource Manager tool exists
# Input        : None
# Return       : (1=Succeded, 0=Failed)
# Stdout       : None
# Stderr       : None
#-------------------------------------------------------------------------------

sub CheckResourceManager
{
  GetDataMartHome() if (not defined $DATAMART_HOME);
  GetDataLoadHome() if (not defined $DATALOAD_HOME);

  if ( -x "$DATAMART_HOME/bin/resmgr" )
  {
    return 1;
  }
  if ( -x "$DATALOAD_HOME/bin/resmgr" )
  {
    return 1;
  }
  return 0;
}

#-------------------------------------------------------------------------------
# PVCommon::PrintObjectList
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  : Print an heterogenous list of objects
# Input        : $val [ $level ]
# Return       : None
# Stdout       : Yes
# Stderr       : None
#-------------------------------------------------------------------------------

sub PrintObjectList($;$);
sub PrintObjectList($;$)
{
  my($val)=$_[0];
  my($level)=defined($_[1])?$_[1]:0;

  if (not ref($val))
  {
    print "(VALUE)";
    print "  " x $level;
    print defined($val)?$val:"<undef>";
  }
  if (ref($val) eq "CODE")
  {
    print "(CODE)";
    return;
  }
  if (ref($val) eq "REF")
  {
    print "(REF)";
    PrintObjectList(${$val}, $level);
    return;
  }
  if (ref($val) eq "LVALUE")
  {
    print "(LVALUE)";
    PrintObjectList($val, $level);
    return;
  }
  if (ref($val) eq "SCALAR")
  {
    print "(SCALAR)";
    print "  " x $level;
    print defined(${$val})?${$val}:"<undef>";
  }
  if (ref($val) eq "HASH")
  {
    my (@keys)=sort(keys(%{$val}));
    if ($#keys < 0)
    {
      print "(HASH){empty}";
    }
    foreach my $key (@keys)
    {
      print "(HASH)";
      print "  " x $level;
      if (not ref($val->{ $key }))
      {
        print $key . ":(VALUE)" . (defined($val->{ $key })?$val->{ $key }:"<undef>") . "\n";
      }
      else
      {
        print $key . ":\n";
        PrintObjectList($val->{ $key }, $level+1);
      }
    }
  }
  if (ref($val) eq "ARRAY")
  {
    print "(ARRAY)";
    print "  " x $level;
    if ($#{$val} < 0)
    {
      print "[empty]";
    }
    else
    {
      print "[";
      foreach my $v (@{$val})
      {
        if (not ref($v))
        {
          print (defined($v)?$v:"<undef>");
        }
        else
        {
          PrintObjectList($v, $level);
        }
        print " ";
      }
      print "]";
    }
  }
  print "\n";
  return;
}

return 1;

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# END OF CODE
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=head1 NAME

PVCommon - A set of useful procedures dedicated to PROVISO applications.

=head1 SYNOPSIS

        use PVCommon;

        if (not GetDataLoadHome())
        {
                die "Proviso SNMP DataLoad is not correctly installed !";
        }

        if (not GetDataLoadEnv())
        {
                die "Proviso SNMP DataLoad environement is missing !";
        }

        my($config)=GetDataLoadConfig();

        if (not defined $config)
        {
                die "Proviso SNMP DataLoad configuration is missing !";
        }

        StopDataLoad();

        RemoveDataLoadDataDir();

        StartDataLoad();

=head1 DESCRIPTION

This package contains a set of procedures that let you interface
with the Proviso environement.

=head2 OVERVIEW

Most of the function described below are related to DataMart and DataLoad
Modules. The main purpose is to get standard application components like
home or installation directory, unix users, environment variables.
Some of them let you perform actions like stoping and restarting daemons
or modifying directories.

=head2 FUNCTIONS

=head3 COMMON

=over 4

=item PVCommon::strtrim

Remove the blanks characters at the begining and at the end of the given string.

=item PVCommon::IsInTable

Search in the given indexed table a value equal to the given keyword.

=item PVCommon::PrintObjectList

Print an heterogenous list of objects

=item PVCommon::Decrypt

Decrypt Proviso passwords

=back

=head3 DATAMART

=over 4

=item PVCommon::GetDataMartHome

Try to infer the DataMart Home directory

=item PVCommon::GetDataMartUser

Try to infer the DataMart user

=item PVCommon::CheckResourceManager

Check if the Resource Manager tool exists

=back

=head3 DATALOAD

=over 4

=item PVCommon::GetDataLoadHome

Try to infer the DataLoad Home directory

=item PVCommon::GetDataLoadUser

Try to infer the DataLoad user

=item PVCommon::GetDataLoadConfig

Load the configuration file

=item PVCommon::GetDataLoadEnv

Load the environement file and modify the current environment.

=item PVCommon::RemoveDataLoadDataDir

Remove all local statistic data files

=item PVCommon::StartDataLoad

Start the Proviso DataLoad SNMP collector daemon

=item PVCommon::StopDataLoad

Stop the Proviso DataLoad SNMP collector daemon

=back

=head1 COPYRIGHT

Copyright (c) 2000-2004 B<QUALLABY Corporation>. All rights reserved.
This program is not free and is protected by license terms and conditions.
It cannot be distribuated without the agreement of Quallaby.

see also http://www.quallaby.com

=head1 AUTHOR

Alexandre COSTANTINI <acostantini@quallaby.com>

=head1 REPORTING BUGS

If you work for QUALLABY, please contact the author.
If you are a customer, please contact the QUALLABY customer support :

=over 4

=item In North America

Tel: 978-322-4200

<support@quallaby.com>

=item In Europe, Middle East & Asia (EMEA)

Tel: +33 (0) 1 55 60 31 10

<support@quallaby.fr>

=back

=cut

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# END OF DOC
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
