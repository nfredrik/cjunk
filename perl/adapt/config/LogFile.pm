#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
#                     L O G F I L E    P E R L    P A C K
#
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# FileName      : LogFile.pm
# Author        : Alexandre COSTANTINI
# Date          : 30 Aug 2005
# Version       : 1.0
# Subjet        :
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Netcool/Proviso                                        Copyright(c) MICROMUSE
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package LogFile;

#===============================================================================
# Requirements
#===============================================================================

require Exporter;

use POSIX;
use Time::Local;
use Sys::Hostname;
use Net::hostent;
use File::Basename;
use File::Copy;
use File::Path;
use Fcntl qw(:flock);
use strict;

#===============================================================================
# Global variables
#===============================================================================

our(@ISA)=("Exporter");
our($VERSION)="1.01";

#-------------------------------------------------------------------------------
# new
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#   Constructor
# Input        :
#   filename   - Name of the file to truncate
# Return       :
#   $status    - 1=Succeded, 0=Failed
#-------------------------------------------------------------------------------

sub new
{
  my($type)=shift;
  my(%self);
  $self{ 'filename' } = shift || return undef;
  $self{ 'extend'   } = shift || "true";
  $self{ 'logmax'   } = shift || 1024;
  $self{ 'verbose'  } = shift || 0;
  $self{ 'debug'    } = shift || 0;
  $self{ 'datefmt'  } = "%Y/%m/%d %H:%M:%S";
  $self{ 'gmt'      } = 0;
  sysopen(LOGFILE, $self{ 'filename' },O_RDWR|O_APPEND|O_CREAT) || return 0;
  return bless(\%self, $type);
}

#-------------------------------------------------------------------------------
# DESTROY
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#   Deonstructor
# Input        :
#   None
# Return       :
#   None
#-------------------------------------------------------------------------------

sub DESTROY
{
  my $self=shift;
  $self->SUPER::DESTROY if $self->can("SUPER::DESTROY");
  $self=undef;
  close(LOGFILE);
}

#-------------------------------------------------------------------------------
# GetFileName
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
#-------------------------------------------------------------------------------

sub GetFileName()
{
  my($self)=shift;
  return $self->{ 'filename' };
}

#-------------------------------------------------------------------------------
# GetExtend
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
#-------------------------------------------------------------------------------

sub GetExtend()
{
  my($self)=shift;
  return $self->{ 'extend' };
}

#-------------------------------------------------------------------------------
# GetLogMax
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
#-------------------------------------------------------------------------------

sub GetLogMax()
{
  my($self)=shift;
  return $self->{ 'logmax' };
}

#-------------------------------------------------------------------------------
# GetVerbose
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
#-------------------------------------------------------------------------------

sub GetVerbose()
{
  my($self)=shift;
  return $self->{ 'verbose' };
}

#-------------------------------------------------------------------------------
# GetDebug
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
#-------------------------------------------------------------------------------

sub GetDebug()
{
  my($self)=shift;
  return $self->{ 'debug' };
}

#-------------------------------------------------------------------------------
# SetExtend
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
#-------------------------------------------------------------------------------

sub SetExtend()
{
  my($self)=shift;
  $self->{ 'extend' }=shift;
}

#-------------------------------------------------------------------------------
# SetLogMax
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
#-------------------------------------------------------------------------------

sub SetLogMax()
{
  my($self)=shift;
  $self->{ 'logmax' }=shift;
}

#-------------------------------------------------------------------------------
# SetVerbose
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
#-------------------------------------------------------------------------------

sub SetVerbose()
{
  my($self)=shift;
  $self->{ 'verbose' }=shift;
}

#-------------------------------------------------------------------------------
# SetDebug
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
#-------------------------------------------------------------------------------

sub SetDebug()
{
  my($self)=shift;
  $self->{ 'debug' }=shift;
}

#-------------------------------------------------------------------------------
# Flush
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#
# Input        :
#   None
# Return       :
#   $status   - 1=Succeded, 0=Failed
#-------------------------------------------------------------------------------

sub Flush()
{
  my($self)=shift;
  return 0;
}

#-------------------------------------------------------------------------------
# TailLog
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#   Auto scale the size of the log file
# Input        :
#   $filename - Name of the file to truncate
#   $max      - Maximum Ko to tail
# Return       :
#   $status   - 1=Succeded, 0=Failed
#-------------------------------------------------------------------------------

sub TailLog()
{
  my($self)=shift;
  my($filename)=$self->{ 'filename' };
  my($max)=$self->{ 'logmax' }*1024;
  my($total)=(stat($filename))[7];
  my($bytes);
  if ($total > ($max+$max*0.05))  # max + 5% of max
  {
    sysseek(LOGFILE, -$max, 2);
    sysread(LOGFILE, $bytes, $max);
    truncate(LOGFILE, 0);
    my($towrite)=substr($bytes, index($bytes, "\n")+1);
    syswrite(LOGFILE, $towrite);
  }
  return 1;
}

#-------------------------------------------------------------------------------
# PrintLog
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#   Print a log message in logfile and screen if verbose
# Input        :
#   $type     - Type of message (example "warning", "critical")
#   $msg      - Message to display
# Return       :
#   $status   - 1=Succeded, 0=Failed
#-------------------------------------------------------------------------------

sub PrintLog
{
  my($self)=shift;
  my($type)=shift;
  my($msg)=shift;
  my($filename)=$self->{ 'filename' };
  my($progname)=$self->{ 'progname' };
  my($datetime)=strftime($self->{ 'datefmt' },
                        ($self->{ 'gmt' })?gmtime(time):localtime(time));
  my($pid)=$$;

  my($towrite)=sprintf("%s | %+5.5s | %-8.8s | %-.255s\n",
                       $datetime, $pid, $type, $msg);
  printf "$towrite" if ($self->{ 'verbose' });
  flock(LOGFILE, LOCK_EX);
  if (syswrite(LOGFILE, "$towrite"))
  {
    $self->TailLog() if ($self->{ 'extend' } =~ m/false/i);
  }
  flock(LOGFILE, LOCK_UN);
  return 1;
}

#-------------------------------------------------------------------------------
# PrintInfo
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#   Print an Info log message in logfile and screen if verbose
# Input        :
#   @msgs     - Messages to display
# Return       :
#   $status   - 1=Succeded, 0=Failed
#-------------------------------------------------------------------------------

sub PrintInfo
{
  my($self)=shift;
  return $self->PrintLog("Info", join('', @_));
}

#-------------------------------------------------------------------------------
# PrintWarning
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#   Print a Warning log message in logfile and screen if verbose
# Input        :
#   @msgs     - Messages to display
# Return       :
#   $status   - 1=Succeded, 0=Failed
#-------------------------------------------------------------------------------

sub PrintWarning
{
  my($self)=shift;
  return $self->PrintLog("Warning", join('',@_));
}

#-------------------------------------------------------------------------------
# PrintError
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#   Print an Error log message in logfile and screen if verbose
# Input        :
#   @msgs     - Messages to display
# Return       :
#   $status   - 1=Succeded, 0=Failed
#-------------------------------------------------------------------------------

sub PrintError
{
  my($self)=shift;
  return $self->PrintLog("Error", join('',@_));
}

#-------------------------------------------------------------------------------
# PrintDebug
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Description  :
#   Print an Error log message in logfile and screen if verbose
# Input        :
#   @msgs     - Messages to display
# Return       :
#   $status   - 1=Succeded, 0=Failed
#-------------------------------------------------------------------------------

sub PrintDebug
{
  my($self)=shift;
  my($debug)=$self->{ 'debug' };

  if ($debug)
  {
    return $self->PrintLog("Debug", join('',@_));
  }
}

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# END OF CODE
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=head1 NAME

LogFile - This Class manage log files and messages, three kind of messages is
allowed : Info, Warning, Error.

=head1 SYNOPSIS

  use LogFile

  $PLOGFILE=new LogFile("mylog.log", false, 1024, 0, 0, "%d/%m/%y %H:%M:%S", 0);

=head1 DESCRIPTION

=head2 OVERVIEW

=head2 FUNCTIONS

=over 4

=item new( FILENAME, [EXTEND], [LOGMAX], [VERBOSE], [DEBUG], [DATEFMT], [GMT])

Constructor method.

=item DESTROY

Destructor method.

=item PrintInfo

=item PrintWarning

=item PrintError


=back

=head1 COPYRIGHT

Copyright (c) 2005 B<MICROMUSE Ltd.>. All rights reserved.
This program is not free and is protected by license terms and conditions.
It cannot be distribuated without the agreement of Micromuse.

See also http://www.micromuse.com

=head1 AUTHOR

Alexandre COSTANTINI <acostant@micromuse.com>

=head1 REPORTING BUGS

If you work for MICROMUSE, please contact the author.
If you are a customer, please contact the MICROMUSE customer support :

=over 4

=item In North America

Tel: 978-322-4200

<support@micromuse.com>

=item In Europe, Middle East & Africa

Tel: +33 (0) 1 55 60 31 00

<support@micromuse.com>

=back

=cut

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# END OF FILE
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=