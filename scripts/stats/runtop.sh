#!/bin/sh

count=$(($2*$1/120))

#run top 
echo "count is $count"
echo "top at 120 second interval" > $3/top.txt
echo `uname -a `>>$3/top.txt
top d 120 b n $count >> $3/top.txt

