#!/bin/sh

MIX_LOG=$1
LOGFILE=resp_time.log

if [ $# -eq 0 ]; then
	echo "Usage : results2.sh filename"
	exit 1
fi

if [ ! -f $1 ]; then
	echo "$1 : file not found"
	exit 1
fi


#----------------------------------------
# functions
#----------------------------------------
function divide_log()
{
	# $1 : transaction type
	# $2 : mix.log format file
	# $3 : resp_time.log

	awk -F , -v target=$1 'BEGIN{
		FS="," ;
	}
	{
		if ($1 ~ /TERMINATED/) {
			exit;
		}
		if (target == $2) {
			printf("%f\n", $3)
		}
	}' $2 | sort -n > $3 
}

function expr_square_root
{
	# $1 : average to resp_time.log
	# $2 : resp_time.log

	awk -v average=$1 'BEGIN{
		total=0;
	}
	{
		count++;
		total += ($0 - average) ^ 2
	}
	END {
		if(count){
			sqrt_value = sqrt((total / count));
		}
		else {
			sqrt_value = 0;
		}
		print sqrt_value;
	}' $2
}


#----------------------------------------
# Main Function
#----------------------------------------
# Print Headers

echo ""
echo "Response Times(s)"
printf '%-15s   %10s   %10s   %7s  %10s   %10s\n' \
		"Transaction" "90%tile" "10%tile" "Average" "S.Deviation" "Maximum"

TARGET_TRANS=("AC" "AR" "BS" "BC" "BR" "CR" "HO" "NP" "OD" "OI" "PD" "SR" "SU" "SC")

TRANS_NAME=("Admin-Confirm" "Admin-Request" "Best-Sellers" "Buy-Confirm" "Buy-Request" "Customer-Regist" "Home" "New-Products" "Order-Display" "Order-Inquiry" "Product-Detail" "Search-Request" "Search-Results" "Shopping-Cart")	

# Calculate values of 90percentile/averate/maximum
# per type of transaction

for n in `seq 0 13` ; do 

	# divide mix.log per type of transaction

	divide_log ${TARGET_TRANS[$n]} $MIX_LOG $LOGFILE

	# calculate the value of 90 percentile.

	CNT_NUM=`cat $LOGFILE | wc -l`
	TILE_NR=`expr $CNT_NUM \* 90 / 100`
	if [ ${TILE_NR} -le 0 ]; then
		TILE_NR=1
	fi

	# get the response time of 90 percentile of the whole

	TILE_VAL=`sed -n ${TILE_NR}p $LOGFILE`

	# calculate the value of 10 percentile.

	TENTILE_NR=`expr $CNT_NUM \* 10 / 100`
	if [ ${TENTILE_NR} -le 0 ]; then
		TENTILE_NR=1
	fi

	# get the response time of 10 percentile of the whole

	TENTILE_VAL=`sed -n ${TENTILE_NR}p $LOGFILE`

	# get average value of whole per type of transaction

	AVG_VAL=`awk '
		{
			total+=$1;
			count++;
		}
		END {
			if(count){
				printf ("%0.3f", (total / count))
			}
			else {
				printf("0.000");
			}
		}' $LOGFILE`

	# get square root value per type of transaction

	SQRT_VAL=`expr_square_root $AVG_VAL $LOGFILE`

	# get max value of whole per type of transaction

	MAX_VAL=`awk 'END{print $1}' $LOGFILE`

	# print each value

	printf '%-15s   %10.6f   %10.6f   %7.3f   %10.3f   %10.6f\n' \
			${TRANS_NAME[$n]} $TILE_VAL $TENTILE_VAL $AVG_VAL $SQRT_VAL $MAX_VAL

done

/bin/rm -f $LOGFILE

echo ""

exit 0



