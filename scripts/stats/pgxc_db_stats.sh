#!/bin/sh

if [ $# -ne 5 ]; then
	echo "usage: db_stats.sh <database_name> <output_dir> <iterations> <sleep> <postgres_server_port>"
	exit
fi

OUTPUT_DIR=$2
ITERATIONS=$3
SAMPLE_LENGTH=$4
SERVER_PORT=$5

echo ===================================================
echo                    COLLECTING DATA PARAMETERS
echo 
echo DATABASE			is $1
echo OUTPUT DIRECTORY		is $2
echo ITARATION			is $3 times
echo SLEEP TIME			is $4 seconds
echo SERVER PORT		is $5
echo ===================================================


COUNTER=0

# put db info into the readme.txt file
psql --version >> $OUTPUT_DIR/readme.txt
echo "the database statistics is taken at $SAMPLE_LENGTH interval and $ITERATIONS count" >> $OUTPUT_DIR/readme.txt
#echo >> $OUTPUT_DIR/readme.txt

# save the database parameters
psql -d $SID1 -p $SERVER_PORT -c "show all"  > $OUTPUT_DIR/param.out

#For PostgreSQL , is this Parameter exits??
# record data and log devspace space information before the test
#dbmcli -d $1 -u dbm,dbm -uSQL dbt,dbt -c info data > $OUTPUT_DIR/datadev0.txt
#dbmcli -d $1 -u dbm,dbm -uSQL dbt,dbt -c info log > $OUTPUT_DIR/logdev0.txt

#record indexes
echo "collect index and key infomation"
psql -d $SID1 -p $SERVER_PORT -c "select * from pg_stat_user_indexes;" -o $OUTPUT_DIR/indexes.out

# reset monitor tables

# Is the monitor init taking too much time?
date
echo "starting database statistics collection " $ITERATIONS " times by sleep " $SAMPLE_LENGTH
while [ $COUNTER -lt $ITERATIONS ]; do
	
	# collent ipcs stats
	/usr/bin/ipcs > $OUTPUT_DIR/ipcs${COUNTER}.out
		
	# check lock statistics
	psql -d $SID1 -p $SERVER_PORT -c "select relname,pid, mode, granted from pg_locks, pg_class where relfilenode = relation;" -o  $OUTPUT_DIR/lockstats${COUNTER}.out
	psql -d $SID1 -p $SERVER_PORT -c "select * from pg_locks where transaction is not NULL;" -o $OUTPUT_DIR/tran_lock${COUNTER}.out

	
	# read the database activity table
	psql -d $SID1 -p $SERVER_PORT -c "select * from pg_stat_activity;" -o $OUTPUT_DIR/db_activity${COUNTER}.out
	# database load
	psql -d $SID1 -p $SERVER_PORT -c "select * from pg_stat_database where datname='DBT1';" -o $OUTPUT_DIR/db_load${COUNTER}.out
	# table info
	psql -d $SID1 -p $SERVER_PORT -c "select relid, relname, heap_blks_read, heap_blks_hit, idx_blks_read, idx_blks_hit from pg_statio_user_tables;" -o $OUTPUT_DIR/table_info${COUNTER}.out
	psql -d $SID1 -p $SERVER_PORT -c "select  relid, indexrelid, relname, indexrelname, idx_blks_read, idx_blks_hit from pg_statio_user_indexes;" -o $OUTPUT_DIR/index_info${COUNTER}.out
	# scans 
	psql -d $SID1 -p $SERVER_PORT -c "select * from pg_stat_user_tables;" -o $OUTPUT_DIR/table_scan${COUNTER}.out
	psql -d $SID1 -p $SERVER_PORT -c "select * from pg_stat_user_indexes;" -o $OUTPUT_DIR/indexes_scan${COUNTER}.out

	let COUNTER=COUNTER+1
	sleep $SAMPLE_LENGTH
done
