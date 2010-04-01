#!/bin/sh
#********************************************************#
# Response time Least-squares method Script              #
#                               for mix.log format       #
#          Make by Takushi.Shirai                        #
#                  Chika.Sato                            #
#--------------------------------------------------------#
# $Revision: 1.1 $ 
# $Date: 2008/04/08 10:03:43 $
#********************************************************#

SPLIT_COUNT=5
SCALE=0
STIME=1
ETIME=0
G_FLAG=0
Y_VAL=

while getopts GY: OPT ; do
	case $OPT in
	G)
		G_FLAG=1
		;;
	Y)
		Y_VAL="$OPTARG"
		;;
	*)
		echo "Error : Options are different"
		exit 1
		;; 
	esac
done

shift $((OPTIND - 1))

if [ $# -eq 0 ] ; then
	echo "Usage : results3.sh [-G] [-Y value] filename [end time]"
	echo "        results3.sh [-G] [-Y value] filename [start time] [end time]"
	exit 1
fi

if [ ! -f $1 ] ;then
	echo " $1 : file not found \n"
	exit 1
fi

if [ $# -eq 2 ] ; then
	ETIME=$2
elif [ $# -eq 3 ] ; then
	STIME=$2
	ETIME=$3
fi

func_leastSquares()
{
	# $1 : x,y plot data file

	awk 'BEGIN {
		a = b = B = C = D = E = 0.0;
	}
	{
		B += $1 * $1;
		C += $2;
		D += $1 * $2;
		E += $1;
	}
	END {
		a = ((NR * D) - (C * E)) / ((NR * B) - (E * E));
		b = ((B * C) - (D * E)) / ((NR * B) - (E * E));
		printf("%.4f %.4f\n", a * 10000, b);
	}' $1
}

func_transaction()
{
	# $1 : mix.log format file
	# $2 : transaction type

	awk -v type=$2 'BEGIN {
		FS=",";
	}

	{
		if ($1 ~ /TERMINATED/) {
			exit;
		}
		if ($2 != type) {
			next;
		}
		if (save_key == 0) {
			start_sec = $1;
			save_key = $1;
			resp += $3;
			count++;
			next;
		}
		if (save_key != $1) {
			if (count) {
				printf("%d %f\n", save_key - start_sec + 1, resp / count);
			}
			save_key = $1;
			resp = 0;
			count = 0;
		}
		resp += $3;
		count++;
	}

	END {
		if (count) {
			printf("%d %f\n", save_key - start_sec + 1, resp / count);
		}
	}' $1
}

func_split()
{
	# $1 : start second
	# $2 : stop second

	awk 'BEGIN {
		Start = ARGV[2]; delete ARGV[2];
		Stop = ARGV[3]; delete ARGV[3];
	}
	{
		if ($1 >= Start) {
			print $0;
		}
		if ($1 >= Stop) {
			exit 0;
		}
	}' resp_avg.log $1 $2
}

plot_graph()
{
	# $1 : File to plot a graph
	# $2 : transaction type
	# $3 : transaction name

	cat << EOF | gnuplot
	plot "$1" title "$3" with lines
	replot "$1" smooth bezier title "bezier line" with lines
	set xlabel "Elapsed Time(sec)"
	set ylabel "Avg Response Tiem(sec)"
	set term png
	set output "response_avg_$2.png"
	set xrange [0:]
	set yrange [0:${Y_VAL}]
	replot
EOF
}

main_one_transaction()
{
	# $1 : mix.log format file
	# $2 : transaction type
	# $3 : transaction name
	# $4 : start time scale (scale)
	# $5 : end time scale (ecale)

	func_transaction $1 $2 > resp_avg.log

	if [ "$5" -ne 0 ] ; then
		awk -v scale=$4 -v ecale=$5 '{
			if ($1 >= scale && $1 <= ecale) {
				printf("%d %f\n", $1-(scale-1), $2);
			}
		}' resp_avg.log > scale.log
		mv scale.log resp_avg.log
	fi

	FLINE=`wc -l resp_avg.log | awk '{ print $1 }'`
	if [ "$FLINE" -le "1" ]; then
		printf "%-15s  not response data\n" $3
		return
	fi

	SLOPE=`func_leastSquares resp_avg.log`

	AA=`echo $SLOPE | awk '{ print $1 }'`
	BB=`echo $SLOPE | awk '{ print $2 }'`

	printf "%-14s %9.3f  %8.3f" $3 $AA $BB


	if [ "$G_FLAG" == 1 ] ; then
		plot_graph resp_avg.log $2 $3
	fi


	TOTAL=`tail -1 resp_avg.log | awk '{ print $1 }'`
	if [ "$SCALE" -lt "$TOTAL" ]; then
		SCALE=$TOTAL
	fi
	C1=$((TOTAL / SPLIT_COUNT))

	CNT=0
	for i in `seq 1 $SPLIT_COUNT` ; do
		func_split $CNT $((C1 * i)) > resp_avg${i}.log
		FLINE=`wc -l resp_avg${i}.log | awk '{ print $1 }'`
		if [ "$FLINE" -le "1" ]; then
			printf " %8s" "N/A"
			continue
		fi
		SLOPE=`func_leastSquares resp_avg${i}.log`
		AA=`echo $SLOPE | awk '{ print $1 }'`
		printf " %8.3f" $AA
		/bin/rm -f resp_avg${i}.log
		CNT=$((C1 * i))
	done

	printf "\n"

	/bin/rm -f resp_avg.log
}


#****************************************#
# start main							*#
#****************************************#
printf "Response time Least-squares method  [ Ideal type : y=ax+b (a:E-4) ]\n"
printf "Transaction     Slope(a) Height(b)      1st      2nd      3rd      4th      5th\n"

tran=(AC AR BS BC BR CR HO NP OD OI PD SR SU SC)
tname=("Admin-Confirm" "Admin-Request" "Best-Sellers" "Buy-Confirm" "Buy-Request" "Customer-Regis" "Home" "New-Products" "Order-Display" "Order-Inquiry" "Product-Detail" "Search-Request" "Search-Results" "Shopping-Cart")

for n in `seq 0 13` ; do
	main_one_transaction $1 ${tran[$n]} ${tname[$n]} $STIME $ETIME
done

printf "Scale(x) : %d(sec)  " $SCALE 
if [ $ETIME -ne 0 ] ; then
	printf "[ Start : %d  End : %d ]" $STIME $ETIME
fi

printf "\n"

exit 0
