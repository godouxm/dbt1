#!/bin/sh

#------------------------------------------
# Check arguments
#------------------------------------------
if [ $# -lt 2 ]; then
	echo "please specify duration(second)";
	echo "Usage: extract.sh start_time(s) end_time(s)"
	exit 1;
fi

#----------------------------------------
# Main Function
#----------------------------------------
awk -F, -v start=$1 -v end=$2 '
BEGIN {
	first_flag = 0;
}
{
	if ($0 ~ /START/) {
		first_flag = 1;
		next;
	}
	if (first_flag == 1) { 
		start_time = $1 + start;
		end_time = $1 + end;
		first_flag = 0;
	}
	if ($1 >= start_time && $1 <= end_time) {
		print $0;
	}
}' mix.log >> $1_$2_mix.log
if [ $? != 0 ]; then
	echo "error: awk"
	exit 1
fi

exit 0;
