#!/bin/bash
cd /sys/firmware/memmap
for dir in * ; do
    start=$(cat $dir/start)
    end=$(cat $dir/end)
    type=$(cat $dir/type)
    printf "%016x-%016x (%s)\n" $start $[ $end +1] "$type"
done
