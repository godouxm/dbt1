#! /bin/bash

# Default
HOSTNAME=localhost
DBPORT=5432
USERNAME=pgsql
PASSWORD=pgsql
DBNAME=DBT1

TOOL_ROOT=`dirname $0`
. ${TOOL_ROOT}/wrapper_common.sh

#----------------------------------------------
# Check arguments
#----------------------------------------------
ODBC_FLAG=0
while getopts "c:e:i:f:o:b:h:p:u:s:d:" opt; do
	case $opt in
	c) DB_CONNECTION=$OPTARG ;;
	e) EU=$OPTARG ;;
	i) ITEM=$OPTARG ;;
	f) CONF_DIR=$OPTARG ;;
	o) CLIENT_OUTPUT_DIR=$OPTARG ;;
	b) ODBC_FLAG=1 ;;
	h) HOSTNAME=$OPTARG ;;
	p) DBPORT=$OPTARG ;;
	u) USERNAME=$OPTARG ;;
	s) PASSWORD=$OPTARG ;;
	d) DBNAME=$OPTARG ;;
	esac
done

if [ "${DB_CONNECTION}" = "" \
	-o "${EU}" = "" \
	-o "${ITEM}" = "" \
	-o "${CONF_DIR}" = "" \
	-o "${CLIENT_OUTPUT_DIR}" = "" ]; then
	echo "Usage: appServer_wrapper.sh [-c #] [-e #] [-i #]"
	echo "            -f [config_dir] -o [client_output_dir] [-b]"
	echo "          (if libpq then)"
	echo "            [-h <hostname>] [-p #] [-u <username>]"
	echo "            [-s <password>] [-d <dbname>]"
	echo ""
	echo "    -c #: number of database connections"
	echo "    -e #: number of emurated user"
	echo "    -i #: number of item"
	echo "    -f [config_dir]: wrapper_eu#_cn#.conf install directory"
	echo "    -o [client_output_dir]: output directory (appServer per client)"
	echo "    -b : Select ODBC API"
	echo "  (if libpq then)"
	echo "    -h <hostname>: dbserver name(def.=localhost)"
	echo "    -p #: database port number(def.=5432)"
	echo "    -u <username>: database user name(def.=pgsql)"
	echo "    -s <password>: database user password(def.=pgsql)"
	echo "    -d <dbname>: database name(def.=DBT1)"
	exit 1
fi

if [ ! -e ${CONF_DIR} ]; then
	log_error "wrapper config error: ${CONF_DIR} doesn't exist"
	exit 1
fi

WRAPPER_CONF_FILE="${CONF_DIR}/wrapper_eu${EU}_cn${DB_CONNECTION}.conf"
if [ ! -e ${WRAPPER_CONF_FILE} ] ; then
	log_error "wrapper config error : ${WRAPPER_CONF_FILE} doesn't exist"
	exit 1
fi

#-----------------------------------------------------------------------
# Main Function
#-----------------------------------------------------------------------

RUN_TOOL="appServer"
PAIR_TOOL="dbdriver"

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
	DIVIDE_DB_CONNECTION=`echo "${RUN_TOOL_LIST}" | sed -n ${CNT}p \
			| awk '{ printf( "%d", $5) ;}'`

	if [ "${RUN_HOST}" = "" -o \
		 "${INIT_SCRIPT}" = "" -o \
		 ${DIVIDE_EMULATE_USER} -le 0 -o \
		 ${DIVIDE_DB_CONNECTION} -le 0 ] ; then
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

	# if ODBC
	if [ ${ODBC_FLAG} = 0 ]; then
		log_info "# libpq mode"
		log_info "${DBT1ROOT}/${RUN_TOOL}/${RUN_TOOL} --db_connection ${DIVIDE_DB_CONNECTION} --txn_q_size ${DIVIDE_EMULATE_USER} --txn_array_size ${DIVIDE_EMULATE_USER} --item_count ${ITEM} --host ${HOSTNAME} --dbport ${DBPORT} --username ${USERNAME} --password ${PASSWORD} --dbname ${DBNAME} > ${CLIENT_OUTPUT_DIR}/${RUN_TOOL}.log &"

		# execute ${RUN_TOOL}
		ssh ${RUN_USER}@${RUN_HOST} ". ${INIT_SCRIPT}; pushd ${CLIENT_OUTPUT_DIR} > /dev/null 2>&1; ${DBT1ROOT}/${RUN_TOOL}/${RUN_TOOL} --db_connection ${DIVIDE_DB_CONNECTION} --txn_q_size ${DIVIDE_EMULATE_USER} --txn_array_size ${DIVIDE_EMULATE_USER} --item_count ${ITEM} --host ${HOSTNAME} --dbport ${DBPORT} --username ${USERNAME} --password ${PASSWORD} --dbname ${DBNAME} > ${CLIENT_OUTPUT_DIR}/${RUN_TOOL}.log &" &
	else
		log_info "# odbc mode"
		log_info "${DBT1ROOT}/${RUN_TOOL}/${RUN_TOOL} --username ${PGUSER} --db_connection ${DIVIDE_DB_CONNECTION} --txn_q_size ${DIVIDE_EMULATE_USER} --txn_array_size ${DIVIDE_EMULATE_USER} --item_count ${ITEM} > ${CLIENT_OUTPUT_DIR}/${RUN_TOOL}.log &"

		# execute ${RUN_TOOL}
		ssh ${RUN_USER}@${RUN_HOST} ". ${INIT_SCRIPT}; pushd ${CLIENT_OUTPUT_DIR} > /dev/null 2>&1; ${DBT1ROOT}/${RUN_TOOL}/${RUN_TOOL} --username ${PGUSER} --db_connection ${DIVIDE_DB_CONNECTION} --txn_q_size ${DIVIDE_EMULATE_USER} --txn_array_size ${DIVIDE_EMULATE_USER} --item_count ${ITEM} > ${CLIENT_OUTPUT_DIR}/${RUN_TOOL}.log & " &
	fi
	EXECUTE_PID="${EXECUTE_PID} $!"

	if [ $? != 0 ]; then
		log_error "execute ssh ${RUN_TOOL} (${RUN_USER}@${RUN_HOST}). check ${RUN_TOOL} error.log file"
		kill_script_all ${RUN_TOOL} ${RUN_USER}
		exit 1
	fi
	EXECUTE_RUN_HOST="${EXECUTE_RUN_HOST} ${RUN_HOST}"
done

# check threads active (db_connection)
SERVER_ACTIVE=0
while true
do
	sleep 10
	for CHECK_PID in ${EXECUTE_PID}
	do
		kill -0 ${CHECK_PID} >/dev/null 2>&1
		if [ $? -ne 0 ] ; then
			log_error "${RUN_TOOL} process (${CHECK_PID}) is not found"
			log_error "check ${RUN_TOOL} error.log file"
			kill_script_all ${RUN_TOOL} ${RUN_USER} >/dev/null 2>&1
			exit 1
		fi
	done
	if [ ${SERVER_ACTIVE} = 0 ] ; then
		ACTIVE_COUNT=0
		HOST_COUNT=`echo ${EXECUTE_RUN_HOST} | awk '{print NF}'`
		for RUN_HOST in ${EXECUTE_RUN_HOST}
		do
			IS_ACTIVE=`ssh ${RUN_USER}@${RUN_HOST} \
				"grep 'The app server is active' ${CLIENT_OUTPUT_DIR}/${RUN_TOOL}.log | wc -l"`
			if [ $? != 0 ] ; then
				log_error "can't get ${LOG_NAME} (${RUN_USER}@${RUN_HOST})"
			fi
			if [ ${IS_ACTIVE} -ge 1 ] ; then
				ACTIVE_COUNT=`expr ${ACTIVE_COUNT} + 1`
			fi
		done
		if [ "${ACTIVE_COUNT}" = "${HOST_COUNT}" ] ; then
			log_info "The app server is active..."
			SERVER_ACTIVE=1
		else
			SERVER_ACTIVE=0
		fi
	else
		break
	fi
done

# check process alive
# IS_FINISHED=0
# while [ ${IS_FINISHED} = 0 ]
# do
# 	sleep 30
# 	for CHECK_PID in ${EXECUTE_PID}
# 	do
# 		kill -0 ${CHECK_PID} >/dev/null 2>&1
# 		if [ $? -ne 0 ] ; then
# 			# log_warn "${RUN_TOOL} process (${CHECK_PID}) is not found"
# 			# log_warn "reason may be because ${PAIR_TOOL} was finished normally"
# 			# log_warn "please check ${RUN_TOOL} error.log file"
# 			kill_script_all ${RUN_TOOL} ${RUN_USER} >/dev/null 2>&1
# 			IS_FINISHED=1
# 			break
# 		fi
# 	done
# done

# wait
for CHECK_PID in ${EXECUTE_PID}
do
	kill -0 ${CHECK_PID} >/dev/null 2>&1
	if [ $? -eq 0 ] ; then
		wait ${CHECK_PID}
	fi
done

kill_script_all ${RUN_TOOL} ${RUN_USER} >/dev/null 2>&1
exit 0
