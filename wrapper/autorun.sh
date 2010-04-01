#!/bin/sh

DB_SERVER=DL580G5
PGDATA=/array1/data
PGXLOG=/array2/pg_xlog # use "initdb -X /xxx/xxx"
ARCHIVELOG=/array3/archive_log
TAR_FILENAME=/array3/data_i10000_c15000.tar # full pass
REBOOT_FLAG=1 # before measurement machine reboot. on:1 off:0
PGUSERHOME=/home/pgsql # DBserver side.
WRAPPER_DIR=`pwd`
ITEM_NUM=10000
CUST_NUM=15000
THINKING_TIME=7.2
PORT_NUM=5432
USER_NAME=pgsql
USER_PASS=pgsql
DB_NAME=DBT1

# arguments
CASE_NAME=$1
EU_NUM=$2
CONN_NUM=$3
TEST_DURATION=$4
PRE_TEST_DURATION=$5

##### functions ############################################
# Machin Reboot Function
function server_reboot()
{
	# DBServer Reboot
	echo "# ---`date`: --- DBServer Reboot ---"
	ssh ${DB_SERVER} "sudo reboot"
	if [ $? != 0 ]; then
		echo "ssh error: DBServer Reboot"
		exit 1
	fi

	# Wait until DB Server rebooted
	echo "# ---`date`: --- Wait until DB Server rebooted ---"
	sleep 180
	while : ; do
		sleep 10
		ping ${DB_SERVER} -c 5 > /dev/null 2>&1
		if [ $? = 0 ]; then
			break
		fi
	done
	sleep 180
}

##### main #################################################
echo "# ===`date`: === START ${CASE_NAME} ==="

# Stop PostgreSQL
echo "# ---`date`: --- Stop PostgreSQL ---"
ssh ${DB_SERVER} \
	"source .bash_profile; \\
	pg_ctl -w stop -D ${PGDATA}"
if [ $? != 0 ]; then
	echo "ssh error: Stop PostgreSQL(do not mind it)"
fi
# Remove PGDATA
echo "# ---`date`: --- Remove PGDATA ---"
(time ssh ${DB_SERVER} \
	"source .bash_profile; \\
	rm -rf ${PGDATA}; \\
	rm -rf ${PGXLOG}; \\
	rm -rf ${ARCHIVELOG};")
if [ $? != 0 ]; then
	echo "ssh error: Remove PGDATA"
	exit 1
fi
# Make PGDATA Dir
echo "#--- `date` --- Make PGDATA Dir ---"
ssh ${DB_SERVER} \
	"source .bash_profile; \\
	mkdir ${PGXLOG}; \\
	mkdir ${ARCHIVELOG}; \\
	mkdir -p ${PGDATA} ; \\
	chmod 700 ${PGDATA} ; \\
	pushd ${PGDATA} > /dev/null 2>&1; \\
	if [ -d pg_xlog ]; then \\
		ln -s ${PGXLOG} pg_xlog; \\
	fi \\
	popd > /dev/null 2>&1;"
if [ $? != 0 ]; then
	echo "ssh error: Make PGDATA Dir"
	exit 1
fi
# Expand tar
echo "#--- `date` --- Expand tar ${TAR_FILENAME} ---"
(time ssh ${DB_SERVER} \
	"pushd ${PGDATA} > /dev/null 2>&1; \\
	cd ..; \\
	/bin/tar xvf ${TAR_FILENAME} > /dev/null; \\
	popd > /dev/null 2>&1;")
if [ $? != 0 ]; then
	echo "ssh error: Expand tar"
	exit 1
fi

# DBserevr Reboot
if [ "${REBOOT_FLAG}" == 1 ]; then
	(time server_reboot)
fi

# Start PostgreSQL
echo "# ---`date`: --- Start PostgreSQL ---"
ssh ${DB_SERVER} \
	"source .bash_profile; \\
	pg_ctl -w start -D ${PGDATA} -l ${PGUSERHOME}/log;"
if [ $? != 0 ]; then
	echo "ssh error: pg_ctl -w start failed"
	exit 1
fi

sleep 10

# Execute Wrapper
./dbt1_wrapper.sh	-i ${ITEM_NUM} -u ${CUST_NUM} -t ${THINKING_TIME} \
					-f ${WRAPPER_DIR} -h ${DB_SERVER} -P ${PORT_NUM} \
					-U ${USER_NAME} -s $USER_PASS} -D ${DB_NAME} \
					-e ${EU_NUM} -c ${CONN_NUM} -d ${TEST_DURATION} -p ${PRE_TEST_DURATION} \
					-o ${WRAPPER_DIR}/${CASE_NAME}/appServer \
					-O ${WRAPPER_DIR}/${CASE_NAME}/dbdriver \
					-r ${WRAPPER_DIR}/${CASE_NAME}/db_log

sleep 10

# Stop PostgreSQL
echo "# ---`date`: --- Stop PostgreSQL ---"
ssh ${DB_SERVER} \
	"source .bash_profile; \\
	pg_ctl -w stop -D ${PGDATA}"
if [ $? != 0 ]; then
	echo "ssh error: Stop PostgreSQL"
	exit 1
fi

sleep 10

# Get DBServer /var/log/messages
ssh ${DB_SERVER} "sudo tar czvf ${WRAPPER_DIR}/${CASE_NAME}/db_log/var_log_massages.tar.gz /var/log/messages"

echo "# ===`date`: === END ${CASE_NAME} ==="

exit 0

