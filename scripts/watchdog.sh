#!/bin/sh
config=/tmp/watchload.config
myname="${0##*/} (PID $$)"

# Function to set $maxload from $config:
read_config()
{
  if [ "$1" == -v ]
  then echo "$myname: reading $config file"
  fi
  . $config
}

read_config                 # set $maxloads array
trap 'read_config -v' HUP   # On SIGHUP, re-read $config

# Endless loop until killed by signal 15 (etc.):
while :
do
  loads=$(< /proc/loadavg)  # get system loads
  load1=${loads%%.*}        # get 1-minute load (integer)
  if [[ load1 -ge maxload ]]
  then echo "$myname: 1-minute load is $load1 at $(date)"
  fi
  sleep 60
done
