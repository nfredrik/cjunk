#!/usr/bin/sh -x

#
# kill some processes after hanging
#

for  proc in `ps -edaf | egrep '(osesys|rpcon|lnheth|api)' | awk '/'$USER'/ {print $2 }`
   do 
     echo "killing $proc"
     kill -9 $proc
   done