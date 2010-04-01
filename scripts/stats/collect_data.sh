#!/bin/sh
if [ $# -ne 4 ]; then
	echo "Usage: ./collect_data.sh <interval> <count> <cpus> <result_dir>"
	exit
fi

INTERVAL=$1
COUNT=$2
CPUS=$3
RESULTS_PATH=$4
RUN_DURATION=$(($INTERVAL * $COUNT))

echo ===================================================
echo                    COLLECTING DATA PARAMETERS
echo
echo INTERVAL  is $INTERVAL seconds
echo COUNT      is $COUNT times
echo CPU           is $CPUS
echo RESULT PATH   is $RESULT_PATH
echo RUN DURATION is $RUN_DURATION seconds   
echo ===================================================


#get database statistics
./db_stats.sh $SID1 $RESULTS_PATH $COUNT $INTERVAL &

if [ -f ./run.sar.data ]; then
	rm ./run.sar.data
fi

#sar -u -U ALL -d -b -B -r -q -W -o run.sar.data $1 $2 &
#sar -u -I ALL -P ALL -d -B -r -W -o run.sar.data $1 $2 &
sar -A -o run.sar.data $1 $2 &
sh ./io.sh $1 $2 $RESULTS_PATH &
sh ./runtop.sh $1 $2 $RESULTS_PATH &

#What is clearprof...
#clearprof
echo "sleep for $RUN_DURATION seconds..."
sleep $RUN_DURATION

#What is getprof...
#getprof
echo "cpu is $CPUS"
echo "Cpu Statistics (sar -u ) ">$RESULTS_PATH/all.cpu.txt
echo `uname -a `>>$RESULTS_PATH/all.cpu.txt
sar -u -f run.sar.data | tee -a ./run.sar.data.cpu  >> $RESULTS_PATH/all.cpu.txt

#reformatting the cpu data files
i=0
while [ "$i" -lt "$CPUS" ]
do 
	echo "reformatting cpu$i"
	sleep 2
	echo "CPU Statistics CPU$i (sar -u)" > $RESULTS_PATH/cpu$i.csv
	echo `uname -a` >> $RESULTS_PATH/cpu$i.csv
	echo "%user,%nice,%system,%idle">>$RESULTS_PATH/cpu$i.csv
        sar -u -I ALL -f run.sar.data | tee -a ./run.sar.data.cpu.$i | awk '{ if (NR>2) { if ($1!="Average:" && $3=="'$i'") { print $4","$5","$6","$7;} else { if ($1=="Average:" && $2=="'$i'") { print $3","$4","$5","$6;}}}}'>>$RESULTS_PATH/cpu$i.csv
	i=$(($i+1))
done

#reformatting the io data files
echo "reformatting io data files"
for i in `iostat | egrep '^dev' | awk '{print $1}'`
do
        echo "Disk IO (iostat -d) " > $RESULTS_PATH/$i.csv
        echo `uname -a` >>$RESULTS_PATH/$i.csv
        echo "tps,blk_read/s,blk_wrtn/s,blk_read,blk_wrtn" >>$RESULTS_PATH/$i.csv
        cat $RESULTS_PATH/io.txt| grep "$i" | awk '{ print $2","$3","$4","$5","$6}' >>$RESULTS_PATH/$i.csv
done

#reformatting the paging data files
echo "reformatting paging data files"
echo "Paging (sar -B) " > $RESULTS_PATH/paging.txt
echo "Paging (sar -B) " > $RESULTS_PATH/paging.csv
echo `uname -a` >>$RESULTS_PATH/paging.txt
echo `uname -a` >>$RESULTS_PATH/paging.csv
sar -B -f run.sar.data | tee -a ./run.sar.data.page | tee -a $RESULTS_PATH/paging.txt | awk '{ if (NR>2) { if ($1!="Average:") print $3","$4","$5","$6","$7","$8; else { print $2","$3","$4","$5","$6","$7};}}'>>$RESULTS_PATH/paging.csv

#reformatting the memory data files
echo "reformatting memory data files"
echo "Memory (sar -r) "  > $RESULTS_PATH/memory.txt
echo "Memory (sar -r) "  > $RESULTS_PATH/memory.csv
echo `uname -a` >>$RESULTS_PATH/memory.txt
echo `uname -a` >>$RESULTS_PATH/memory.csv
sar -r -f run.sar.data | tee -a ./run.sar.data.mem | tee -a $RESULTS_PATH/memory.txt | awk '{ if (NR>2) { if ($1!="Average:") print $3","$4","$5","$6","$7","$8","$9","$10","$11; else { print $2","$3","$4","$5","$6","$7","$8","$9","$10};}}'>>$RESULTS_PATH/memory.csv

#reformatting the processor queue data files
echo "reformatting queue data files"
echo "Load (sar -q) "  > $RESULTS_PATH/queue.txt
echo "Load (sar -q) "  > $RESULTS_PATH/queue.csv
echo `uname -a` >>$RESULTS_PATH/queue.txt
echo `uname -a` >>$RESULTS_PATH/queue.csv
sar -q -f run.sar.data | tee -a ./run.sar.data.queue | tee -a $RESULTS_PATH/queue.txt | awk '{ if (NR>2) { if ($1!="Average:") { print $3","$4","$5","$6;} else { print $2","$3","$4","$5;}}}'>>$RESULTS_PATH/queue.csv

#reformatting the processor queue data files
echo "reformatting swap data files"
echo "Swap (sar -W)" >$RESULTS_PATH/swap.txt
echo "Swap (sar -W)" >$RESULTS_PATH/swap.csv
echo `uname -a` >>$RESULTS_PATH/swap.txt
echo `uname -a` >>$RESULTS_PATH/swap.csv
sar -W -f run.sar.data | tee -a ./run.sar.data.swap | tee -a $RESULTS_PATH/swap.txt | awk '{ if (NR>2) { if ($1!="Average:") {print $3","$4;} else { print $2","$3;}}}'>>$RESULTS_PATH/swap.csv

#../../tools/results --mixfile ../../dbdriver/mix.log --outputdir $RESULTS_PATH > $RESULTS_PATH/BT
