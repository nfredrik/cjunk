#!/bin/bash
while sleep 5
do
  echo -e "===== $(date) ====="
  netstat | grep tcp
done > netstat.log

