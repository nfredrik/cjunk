#!/bin/sh 
#
# Script to fetch a public ip address and forward it as a file to a ftp server.
#                                           
while true; do
echo -e "GET http://checkip.dyndns.org  HTTP/1.0\n\n" | nc checkip.dyndns.org  80 | awk '/IP Address/ {gsub(/<.*/,"",$6);print $6}' > new_ip_addr

# Check exit status only
if [[ -s old_ip_addr ]] && [[ -s new_ip_addr ]] &&  ! cmp -s old_ip_addr new_ip_addr
then    
  ftpput -u <user> -p <passwd>  <ftpserver hos er> new_ip_addr
fi

if [[ -s new_ip_addr ]]
then    
cp new_ip_addr old_ip_addr
fi

sleep 1h     # eg 30m ( 30 minutes)  1h  (1 hour)
done



