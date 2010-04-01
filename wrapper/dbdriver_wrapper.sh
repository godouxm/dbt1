#! /bin/bash

TOOL_ROOT=`dirname $0`
. ${TOOL_ROOT}/wrapper_common.sh

#----------------------------------------------
# Check arguments
#----------------------------------------------
while getopts "c:i:u:e:t:d:p:f:o:" opt; do
	case $opt in
	c)
		DB_CONNECTION=$OPTARG
		# echo ${DB_CONNECTION}
		;;
	i)
		ITEM=$OPTARG
		# echo ${ITEM}
		;;
	u)
		# CUSTOMER=${OPTARG}
		CUSTOMER=`expr ${OPTARG} \* 2880`
		# echo ${CUSTOMER}
		;;
	e)
		EU=$OPTARG
		# echo ${EU}
		;;
	t)
		THINK_TIME=$OPTARG
		# echo ${THINK_TIME}
		;;
	d)
		REAL_DURATION=$OPTARG
		# echo ${REAL_DURATION}
		;;
	p)
		PRETEST_DURATION=$OPTARG
		# echo ${PRETEST_DURATION}
		;;
	f)
		CONF_DIR=$OPTARG
		# echo ${CONF_DIR}
		;;
	o)
		CLIENT_OUTPUT_DIR=$OPTARG
		# echo ${CLIENT_OUTPUT_DIR}
		;;
	esac
done

if [ "${DB_CONNECTION}" = "" \
	-o "${ITEM}" = "" \
	-o "${CUSTOMER}" = "" \
	-o "${EU}" = "" \
	-o "${THINK_TIME}" = "" \
	-o "${REAL_DURATION}" = "" \
	-o "${PRETEST_DURATION}" = "" \
	-o "${CONF_DIR}" = "" \
	-o "${CLIENT_OUTPUT_DIR}" = "" ]; then
	echo "Usage: dbdriver_wrapper.sh [-i #] [-u #] [-e #] [-t #] [-d #] [-p #]"
	echo "            [-c #] -f [config_dir] -o [client_output_dir]"
	echo ""
	echo "    -i #: number of item"
	echo "    -u #: number of customer"
	echo "    -e #: number of emurated user"
	echo "    -t #: thinking time"
	echo "    -d #: real_duration(s)"
	echo "    -p #: pretest_duration(s)"
	echo "    -c #: number of database connections"
	echo "    -f [config_dir]: wrapper_eu#_cn#.conf install directory"
	echo "    -o [client_output_dir]: output directory (dbdriver per client)"
	exit 1
fi

if [ ! -e ${CONF_DIR} ]; then
	log_error "${CONF_DIR} doesn't exist"
	exit 1
fi

WRAPPER_CONF_FILE="${CONF_DIR}/wrapper_eu${EU}_cn${DB_CONNECTION}.conf"
if [ ! -e ${WRAPPER_CONF_FILE} ] ; then
	log_error "${WRAPPER_CONF_FILE} doesn't exist"
	exit 1
fi

#-----------------------------------------------------------------------
# Main Function
#-----------------------------------------------------------------------

RUN_TOOL="dbdriver"
PAIR_TOOL="appServer"

STAT_TOOL="sar"
STAT_TOOL_ARG="-A 60 0"
CPU_STAT_TOOL="vmstat"
CPU_STAT_TOOL_ARG="1"
IO_STAT_TOOL="iostat"
IO_STAT_TOOL_ARG="-t -x -k 1"

RUN_USER=`whoami`
RUN_USER_HOME=`eval echo ~${RUN_USER}`

RUN_TOOL_LIST=`grep -v "^#" ${WRAPPER_CONF_FILE} \
		| awk '{ if ( tolower($3) == tolower("'${RUN_TOOL}'") ) { printf("%s\n", $0) } }'`
PAIR_TOOL_LIST=`grep -v "^#" ${WRAPPER_CONF_FILE} \
		| awk '{ if ( tolower($3) == tolower("'${PAIR_TOOL}'") ) { printf("%s\n", $0) } }'`
RUN_TOOL_LIST_COUNT=`echo "${RUN_TOOL_LIST}" | wc -l`
PAIR_TOOL_LIST_COUNT=`echo "${PAIR_TOOL_LIST}" | wc -l`

if [ ${RUN_TOOL_LIST_COUNT} -ne ${PAIR_TOOL_LIST_COUNT} ] ; then
	log_error "[param-line] numbers does not match"
	exit 1
fi

EXECUTE_RUN_HOST=""
EXECUTE_PID=""

trap 'kill_script_all ${RUN_TOOL} ${RUN_USER}; exit 1' INT TERM

for (( CNT=1 ; CNT <= RUN_TOOL_LIST_COUNT ; CNT++ )) ; do
	# parse line
	RUN_HOST=`echo "${RUN_TOOL_LIST}" | sed -n ${CNT}p \
			| awk '{ printf( "%s", $1) ;}'`
	INIT_SCRIPT=`echo "${RUN_TOOL_LIST}" | sed -n ${CNT}p \
			| awk '{ printf( "%s", $2) ;}'`
	DIVIDE_EMULATE_USER=`echo "${RUN_TOOL_LIST}" | sed -n ${CNT}p \
			| awk '{ printf( "%d", $4) ;}'`
	TARGET_APPSERVER_HOST=`echo "${RUN_TOOL_LIST}" | sed -n ${CNT}p \
			| awk '{ printf( "%s", $5) ;}'`
	DIVIDE_EU_PER_MIN=`echo "${RUN_TOOL_LIST}" | sed -n ${CNT}p \
			| awk '{ printf( "%d", $6) ;}'`

	if [ "${RUN_HOST}" = "" -o \
		 "${INIT_SCRIPT}" = "" -o \
		 ${DIVIDE_EMULATE_USER} -le 0 -o \
		 "${TARGET_APPSERVER_HOST}" = "" -o \
		 ${DIVIDE_EU_PER_MIN} -le 0 ] ; then
		log_error "param format does not match. ${RUN_TOOL} config (lineno = ${CNT})"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi

	ssh ${RUN_USER}@${RUN_HOST} "test -e ${INIT_SCRIPT}"
	if [ $? != 0 ]; then
		log_error "${INIT_SCRIPT} doesn't exist (${RUN_USER}@${RUN_HOST})"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi

	DBT1ROOT=`ssh ${RUN_USER}@${RUN_HOST} ". ${INIT_SCRIPT}; env | grep '^DBT1ROOT' | cut -d '=' -f 2"`
	if [ "DBT1ROOT" = "" ]; then
		log_error "DBT1ROOT environment variable is not found (${RUN_USER}@${RUN_HOST})"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi

	ssh ${RUN_USER}@${RUN_HOST} "test -e ${DBT1ROOT}"
	if [ $? != 0 ]; then
		log_error "${DBT1ROOT} doesn't exist (${RUN_USER}@${RUN_HOST})"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi

	ssh ${RUN_USER}@${RUN_HOST} "test -e ${CLIENT_OUTPUT_DIR}"
	if [ $? != 0 ]; then
		log_error "${CLIENT_OUTPUT_DIR} doesn't exist (${RUN_USER}@${RUN_HOST})"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi

	# execute ${STAT_TOOL}
	SH_PID=`ssh ${RUN_USER}@${RUN_HOST} "/sbin/pidof ${STAT_TOOL}"`
	if [ $? != 0 -a "${SH_PID}" = "" ] ; then
		ssh ${RUN_USER}@${RUN_HOST} "${STAT_TOOL} ${STAT_TOOL_ARG} -o ${CLIENT_OUTPUT_DIR}/${STAT_TOOL}.out > /dev/null &" &
		if [ $? != 0 ]; then
			log_error "execute ssh ${STAT_TOOL} (${RUN_USER}@${RUN_HOST})"
			kill_script_all ${RUN_TOOL} ${RUN_USER}
			exit 1
		fi
	fi

	# execute ${CPU_STAT_TOOL}
	SH_PID=`ssh ${RUN_USER}@${RUN_HOST} "/sbin/pidof ${CPU_STAT_TOOL}"`
	if [ $? != 0 -a "${SH_PID}" = "" ] ; then
		ssh ${RUN_USER}@${RUN_HOST} "${CPU_STAT_TOOL} ${CPU_STAT_TOOL_ARG} > ${CLIENT_OUTPUT_DIR}/${CPU_STAT_TOOL}.out &" &
		if [ $? != 0 ]; then
			log_error "execute ssh ${CPU_STAT_TOOL} (${RUN_USER}@${RUN_HOST})"
			kill_script_all ${RUN_TOOL} ${RUN_USER}
			exit 1
		fi
	fi

	# execute ${IO_STAT_TOOL}
	SH_PID=`ssh ${RUN_USER}@${RUN_HOST} "/sbin/pidof ${IO_STAT_TOOL}"`
	if [ $? != 0 -a "${SH_PID}" = "" ] ; then
		ssh ${RUN_USER}@${RUN_HOST} "${IO_STAT_TOOL} ${IO_STAT_TOOL_ARG} > ${CLIENT_OUTPUT_DIR}/${IO_STAT_TOOL}.out &" &
		if [ $? != 0 ]; then
			log_error "execute ssh ${IO_STAT_TOOL} (${RUN_USER}@${RUN_HOST})"
			kill_script_all ${RUN_TOOL} ${RUN_USER}
			exit 1
		fi
	fi

	echo ""
	log_info "---- execute ${RUN_TOOL} (HOST=${RUN_HOST}/USER=${RUN_USER}) ----"

	ssh ${RUN_USER}@${RUN_HOST} "echo ''; . ${INIT_SCRIPT}; uname -a; ulimit -a; echo ''"
	if [ $? != 0 ]; then
		log_error "execute ssh uname/ulimit (${RUN_USER}@${RUN_HOST})"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi

	log_info "${DBT1ROOT}/${RUN_TOOL}/${RUN_TOOL} --server_name ${TARGET_APPSERVER_HOST} --item_count ${ITEM} --customer_count ${CUSTOMER} --emulated_users ${DIVIDE_EMULATE_USER} --rampup_rate ${DIVIDE_EU_PER_MIN} --think_time ${THINK_TIME} --duration ${REAL_DURATION} > ${CLIENT_OUTPUT_DIR}/${RUN_TOOL}.log &"

	# execute ${RUN_TOOL}
	ssh ${RUN_USER}@${RUN_HOST} ". ${INIT_SCRIPT}; pushd ${CLIENT_OUTPUT_DIR} > /dev/null 2>&1; ${DBT1ROOT}/${RUN_TOOL}/${RUN_TOOL} --server_name ${TARGET_APPSERVER_HOST} --item_count ${ITEM} --customer_count ${CUSTOMER} --emulated_users ${DIVIDE_EMULATE_USER} --rampup_rate ${DIVIDE_EU_PER_MIN} --think_time ${THINK_TIME} --duration ${REAL_DURATION} > ${CLIENT_OUTPUT_DIR}/${RUN_TOOL}.log &" &
	EXECUTE_PID="${EXECUTE_PID} $!"

	if [ $? != 0 ]; then
		log_error "execute ssh ${RUN_TOOL} (${RUN_USER}@${RUN_HOST}). check ${RUN_TOOL} error.log file"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi
	EXECUTE_RUN_HOST="${EXECUTE_RUN_HOST} ${RUN_HOST}"
done

# wait
for CHECK_PID in ${EXECUTE_PID}
do
	kill -0 ${CHECK_PID} >/dev/null 2>&1
	if [ $? -eq 0 ] ; then
		wait ${CHECK_PID}
	fi
done
kill_script_all ${RUN_TOOL} ${RUN_USER} >/dev/null 2>&1

# Get Results
THROUGHPUT_CALC=0
for RUN_HOST in ${EXECUTE_RUN_HOST}
do
	echo ""
	log_info "---- get results (HOST=${RUN_HOST}/USER=${RUN_USER}) ----"

	# extract
	ssh ${RUN_USER}@${RUN_HOST} ". ${INIT_SCRIPT}; pushd ${CLIENT_OUTPUT_DIR} > /dev/null 2>&1; ${CLIENT_OUTPUT_DIR}/extract_dbt1_log.sh ${PRETEST_DURATION} ${REAL_DURATION}"
	if [ $? != 0 ]; then
		log_error "execute ssh extract_dbt1_log.sh (${RUN_USER}@${RUN_HOST})"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi

	# BT
	ssh ${RUN_USER}@${RUN_HOST} ". ${INIT_SCRIPT}; pushd ${CLIENT_OUTPUT_DIR} > /dev/null 2>&1; ${DBT1ROOT}/tools/results --mixfile ${PRETEST_DURATION}_${REAL_DURATION}_mix.log --outputdir ${CLIENT_OUTPUT_DIR} 2>&1 | tee ${CLIENT_OUTPUT_DIR}/results.out"
	if [ $? != 0 ]; then
		log_error "execute ssh results (${RUN_USER}@${RUN_HOST})"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi
	ssh ${RUN_USER}@${RUN_HOST} "echo ''; cat ${CLIENT_OUTPUT_DIR}/BT"
	if [ $? != 0 ]; then
		log_error "execute ssh cat BT (${RUN_USER}@${RUN_HOST})"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi

	# Execute results2
	ssh ${RUN_USER}@${RUN_HOST} ". ${INIT_SCRIPT}; pushd ${CLIENT_OUTPUT_DIR} > /dev/null 2>&1; ${CLIENT_OUTPUT_DIR}/results2_dbt1.sh ${PRETEST_DURATION}_${REAL_DURATION}_mix.log 2>&1 | tee -a ${CLIENT_OUTPUT_DIR}/results.out"
	if [ $? != 0 ]; then
		log_error "execute ssh results2_dbt1.sh (${RUN_USER}@${RUN_HOST})"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi

	# Execute results3
	ssh ${RUN_USER}@${RUN_HOST} ". ${INIT_SCRIPT}; pushd ${CLIENT_OUTPUT_DIR} > /dev/null 2>&1; ${CLIENT_OUTPUT_DIR}/results3_dbt1.sh -G -Y 10 ${PRETEST_DURATION}_${REAL_DURATION}_mix.log 2>&1 | tee -a ${CLIENT_OUTPUT_DIR}/results.out"
	if [ $? != 0 ]; then
		log_error "execute ssh results3_dbt1.sh (${RUN_USER}@${RUN_HOST})"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi

	# count error
	ssh ${RUN_USER}@${RUN_HOST} "echo -e '\n# wc -l error.log'; wc -l ${CLIENT_OUTPUT_DIR}/error.log"
	if [ $? != 0 ]; then
		log_error "execute ssh wc -l (${RUN_USER}@${RUN_HOST})"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi

	# calculate throughput
	THROUGHPUT=`ssh ${RUN_USER}@${RUN_HOST} "grep 'bogotransactions per second' ${CLIENT_OUTPUT_DIR}/BT"`
	if [ $? != 0 ]; then
		log_error "execute ssh calculate throughput (${RUN_USER}@${RUN_HOST})"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi
	THROUGHPUT=`echo "${THROUGHPUT}" | awk '{ printf("%s", $1) }'`
	THROUGHPUT_CALC=`echo "${THROUGHPUT_CALC} + ${THROUGHPUT}" | bc`

done

echo ""
log_info "######## TOTAL THROUGHPUT ########"
log_info "${THROUGHPUT_CALC} [BT/second] "

exit 0
