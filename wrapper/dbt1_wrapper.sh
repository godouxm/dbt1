#! /bin/bash

function kill_wrapper()
{
	log_info "kill wrapper scripts ...."
	if [ "${EXECUTE_PID}" != "" ]; then
		kill -s TERM ${EXECUTE_PID} >/dev/null 2>&1
	fi
	kill_script ${STAT_TOOL} ${HOSTNAME} ${RUN_USER} >/dev/null 2>&1
	kill_script ${CPU_STAT_TOOL} ${HOSTNAME} ${RUN_USER} >/dev/null 2>&1
	kill_script ${IO_STAT_TOOL} ${HOSTNAME} ${RUN_USER} >/dev/null 2>&1
	return 0
}

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
while getopts "e:i:c:u:t:d:p:f:h:P:U:s:D:o:O:r:" opt; do
	case $opt in
	e)
		EU=$OPTARG ;;
	i)
		ITEM=$OPTARG ;;
	c)
		DB_CONNECTION=$OPTARG ;;
	u)
		CUSTOMER=${OPTARG} ;;
	t)
		THINK_TIME=$OPTARG ;;
	d)
		REAL_DURATION=$OPTARG ;;
	p)
		PRETEST_DURATION=$OPTARG ;;
	f)
		CONF_DIR=$OPTARG ;;
	h)
		HOSTNAME=$OPTARG ;;
	P)
		DBPORT=$OPTARG ;;
	U)
		USERNAME=$OPTARG ;;
	s)
		PASSWORD=$OPTARG ;;
	D)
		DBNAME=$OPTARG ;;
	o)
		APPSERVER_CLIENT_OUTPUT_DIR=$OPTARG ;;
	O)
		DBDRIVER_CLIENT_OUTPUT_DIR=$OPTARG ;;
	r)
		DB_SERVER_OUTPUT_DIR=$OPTARG ;;
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
	-o "${APPSERVER_CLIENT_OUTPUT_DIR}" = "" \
	-o "${DBDRIVER_CLIENT_OUTPUT_DIR}" = "" \
	-o "${DB_SERVER_OUTPUT_DIR}" = "" ]; then
	echo "Usage: dbt1_wrapper.sh [-e #] [-i #] [-c #] [-u #] [-t #] [-d #] [-p #]"
	echo "            -f <config_dir> [-h <hostname>] [-P #] [-U <username>]"
	echo "            [-s <password>] [-D <dbname>]"
	echo "            -o <appServer_output_dir> -O <dbdriver_output_dir>"
	echo "            -r <dbserver_output_dir>"
	echo ""
	echo "    -e #: number of emurated user"
	echo "    -i #: number of item"
	echo "    -c #: number of database connections"
	echo "    -u #: number of customer"
	echo "    -t #: thinking time"
	echo "    -d #: real_duration(s)"
	echo "    -p #: pretest_duration(s)"
	echo "    -f [config_dir]: wrapper_eu#_cn#.conf install directory"
	echo "    -h <hostname>: dbserver name(def.=localhost)"
	echo "    -P #: database port number(def.=5432)"
	echo "    -U <username>: database user name(def.=pgsql)"
	echo "    -s <password>: database user password(def.=pgsql)"
	echo "    -D <dbname>: database name(def.=DBT1)"
	echo "    -o [appServer_output_dir]: output directory (appServer per client)"
	echo "    -O [dbdriver_output_dir]: output directory (dbdriver per client)"
	echo "    -r [dbserver_output_dir]: output directory"
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

APPSERVER_WRAPPER="appServer_wrapper.sh"
DBDRIVER_WRAPPER="dbdriver_wrapper.sh"

APPSERVER_TOOL="appServer"
DBDRIVER_TOOL="dbdriver"

STAT_TOOL="sar"
STAT_TOOL_ARG="-A 60 0"
CPU_STAT_TOOL="vmstat"
CPU_STAT_TOOL_ARG="1"
IO_STAT_TOOL="iostat"
IO_STAT_TOOL_ARG="-t -x -k 1"

RUN_USER=`whoami`
RUN_USER_HOME=`eval echo ~${RUN_USER}`

EXECUTE_PID=""

APPSERVER_TOOL_LIST=`grep -v "^#" ${WRAPPER_CONF_FILE} \
        | awk '{ if ( tolower($3) == tolower("'${APPSERVER_TOOL}'") ) { printf("%s\n", $0) } }'`
DBDRIVER_TOOL_LIST=`grep -v "^#" ${WRAPPER_CONF_FILE} \
        | awk '{ if ( tolower($3) == tolower("'${DBDRIVER_TOOL}'") ) { printf("%s\n", $0) } }'`
APPSERVER_TOOL_LIST_COUNT=`echo "${APPSERVER_TOOL_LIST}" | wc -l`
DBDRIVER_TOOL_LIST_COUNT=`echo "${DBDRIVER_TOOL_LIST}" | wc -l`

trap 'kill_wrapper; exit 1' INT TERM

log_info "######## SETUP  ########"

# setup DB server
# make output dir
ssh ${RUN_USER}@${HOSTNAME} "test -e ${DB_SERVER_OUTPUT_DIR}"
if [ $? = 0 ]; then
	log_error "${DB_SERVER_OUTPUT_DIR} exist (${RUN_USER}@${HOSTNAME})"
	kill_wrapper
	exit 1
fi
ssh ${RUN_USER}@${HOSTNAME} "mkdir -p ${DB_SERVER_OUTPUT_DIR}"
if [ $? != 0 ]; then
	log_error "execute ssh mkdir ${DB_SERVER_OUTPUT_DIR} (${RUN_USER}@${HOSTNAME})"
	kill_wrapper
	exit 1
fi

# setup appServer host
for (( CNT=1 ; CNT <= APPSERVER_TOOL_LIST_COUNT ; CNT++ )) ; do
	# parse line
	RUN_HOST=`echo "${APPSERVER_TOOL_LIST}" | sed -n ${CNT}p \
			| awk '{ printf( "%s", $1) ;}'`
	if [ "${RUN_HOST}" = "" ] ; then
		log_error "param format does not match. ${APPSERVER_TOOL} config (lineno = ${CNT})"
		kill_wrapper
		exit 1
	fi
	# make output dir
	ssh ${RUN_USER}@${RUN_HOST} "test -e ${APPSERVER_CLIENT_OUTPUT_DIR}"
	if [ $? = 0 ]; then
		log_error "${APPSERVER_CLIENT_OUTPUT_DIR} exist (${RUN_USER}@${RUN_HOST})"
		kill_wrapper
		exit 1
	fi
	ssh ${RUN_USER}@${RUN_HOST} "mkdir -p ${APPSERVER_CLIENT_OUTPUT_DIR}"
	if [ $? != 0 ]; then
		log_error "execute ssh mkdir ${APPSERVER_CLIENT_OUTPUT_DIR} (${RUN_USER}@${RUN_HOST})"
		kill_wrapper
		exit 1
	fi
done

# setup dbdriver host
for (( CNT=1 ; CNT <= DBDRIVER_TOOL_LIST_COUNT ; CNT++ )) ; do
	# parse line
	RUN_HOST=`echo "${DBDRIVER_TOOL_LIST}" | sed -n ${CNT}p \
			| awk '{ printf( "%s", $1) ;}'`
	if [ "${RUN_HOST}" = "" ] ; then
		log_error "param format does not match. ${DBDRIVER_TOOL} config (lineno = ${CNT})"
		kill_wrapper
		exit 1
	fi
	# make output dir
	ssh ${RUN_USER}@${RUN_HOST} "test -e ${DBDRIVER_CLIENT_OUTPUT_DIR}"
	if [ $? = 0 ]; then
		log_error "${DBDRIVER_CLIENT_OUTPUT_DIR} exist (${RUN_USER}@${RUN_HOST})"
		kill_wrapper
		exit 1
	fi
	ssh ${RUN_USER}@${RUN_HOST} "mkdir -p ${DBDRIVER_CLIENT_OUTPUT_DIR}"
	if [ $? != 0 ]; then
		log_error "execute ssh mkdir ${DBDRIVER_CLIENT_OUTPUT_DIR} (${RUN_USER}@${RUN_HOST})"
		kill_wrapper
		exit 1
	fi
	# copy script
	scp "${TOOL_ROOT}/extract_dbt1_log.sh" ${RUN_USER}@${RUN_HOST}:"${DBDRIVER_CLIENT_OUTPUT_DIR}/"
	if [ $? != 0 ]; then
		log_error "execute scp extract_dbt1_log.sh (${RUN_USER}@${RUN_HOST})"
		kill_wrapper
		exit 1
	fi
	scp "${TOOL_ROOT}/results2_dbt1.sh" ${RUN_USER}@${RUN_HOST}:"${DBDRIVER_CLIENT_OUTPUT_DIR}/"
	if [ $? != 0 ]; then
		log_error "execute scp results2_dbt1.sh (${RUN_USER}@${RUN_HOST})"
		kill_wrapper
		exit 1
	fi
	scp "${TOOL_ROOT}/results3_dbt1.sh" ${RUN_USER}@${RUN_HOST}:"${DBDRIVER_CLIENT_OUTPUT_DIR}/"
	if [ $? != 0 ]; then
		log_error "execute scp results3_dbt1.sh (${RUN_USER}@${RUN_HOST})"
		kill_wrapper
		exit 1
	fi
done

# setup exec wrapper host (use appServer_wrapper.log and dbdriver_wrapper.log)
# make output dir
test -e ${APPSERVER_CLIENT_OUTPUT_DIR}
if [ $? != 0 ]; then
	mkdir -p ${APPSERVER_CLIENT_OUTPUT_DIR}
	if [ $? != 0 ]; then
		log_error "execute mkdir ${APPSERVER_CLIENT_OUTPUT_DIR}"
		kill_wrapper
		exit 1
	fi
fi
test -e ${DBDRIVER_CLIENT_OUTPUT_DIR}
if [ $? != 0 ]; then
	mkdir -p ${DBDRIVER_CLIENT_OUTPUT_DIR}
	if [ $? != 0 ]; then
		log_error "execute mkdir ${DBDRIVER_CLIENT_OUTPUT_DIR}"
		kill_wrapper
		exit 1
	fi
fi

# execute ${STAT_TOOL} (DB server host)
SH_PID=`ssh ${RUN_USER}@${HOSTNAME} "/sbin/pidof ${STAT_TOOL}"`
if [ $? != 0 -a "${SH_PID}" = "" ] ; then
	ssh ${RUN_USER}@${HOSTNAME} "${STAT_TOOL} ${STAT_TOOL_ARG} -o ${DB_SERVER_OUTPUT_DIR}/${STAT_TOOL}.out > /dev/null &" &
	EXECUTE_PID="${EXECUTE_PID} $!"
	if [ $? != 0 ]; then
		log_error "execute ssh ${STAT_TOOL} (${RUN_USER}@${HOSTNAME})"
		kill_wrapper
		exit 1
	fi
else
	log_error "execute ssh ${STAT_TOOL} (${RUN_USER}@${HOSTNAME})"
	exit 1
fi

# execute ${CPU_STAT_TOOL} (DB server host)
SH_PID=`ssh ${RUN_USER}@${HOSTNAME} "/sbin/pidof ${CPU_STAT_TOOL}"`
if [ $? != 0 -a "${SH_PID}" = "" ] ; then
	ssh ${RUN_USER}@${HOSTNAME} "${CPU_STAT_TOOL} ${CPU_STAT_TOOL_ARG} > ${DB_SERVER_OUTPUT_DIR}/${CPU_STAT_TOOL}.out &" &
	EXECUTE_PID="${EXECUTE_PID} $!"
	if [ $? != 0 ]; then
		log_error "execute ssh ${CPU_STAT_TOOL} (${RUN_USER}@${HOSTNAME})"
		kill_wrapper
		exit 1
	fi
else
	log_error "execute ssh ${CPU_STAT_TOOL} (${RUN_USER}@${HOSTNAME})"
	exit 1
fi

# execute ${IO_STAT_TOOL} (DB server host)
SH_PID=`ssh ${RUN_USER}@${HOSTNAME} "/sbin/pidof ${IO_STAT_TOOL}"`
if [ $? != 0 -a "${SH_PID}" = "" ] ; then
	ssh ${RUN_USER}@${HOSTNAME} "${IO_STAT_TOOL} ${IO_STAT_TOOL_ARG} > ${DB_SERVER_OUTPUT_DIR}/${IO_STAT_TOOL}.out &" &
	EXECUTE_PID="${EXECUTE_PID} $!"
	if [ $? != 0 ]; then
		log_error "execute ssh ${IO_STAT_TOOL} (${RUN_USER}@${HOSTNAME})"
		kill_wrapper
		exit 1
	fi
else
	log_error "execute ssh ${IO_STAT_TOOL} (${RUN_USER}@${HOSTNAME})"
	exit 1
fi

# run all appServer
echo ""
log_info "######## START ${APPSERVER_WRAPPER} ########"
log_info "${TOOL_ROOT}/${APPSERVER_WRAPPER} -c ${DB_CONNECTION} -e ${EU} -i ${ITEM} -h ${HOSTNAME} -p ${DBPORT} -u ${USERNAME} -s ${PASSWORD} -d ${DBNAME} -f ${CONF_DIR} -o ${APPSERVER_CLIENT_OUTPUT_DIR} > ${APPSERVER_CLIENT_OUTPUT_DIR}/appServer_wrapper.log &"

${TOOL_ROOT}/${APPSERVER_WRAPPER} -c ${DB_CONNECTION} -e ${EU} -i ${ITEM} -h ${HOSTNAME} -p ${DBPORT} -u ${USERNAME} -s ${PASSWORD} -d ${DBNAME} -f ${CONF_DIR} -o ${APPSERVER_CLIENT_OUTPUT_DIR} > ${APPSERVER_CLIENT_OUTPUT_DIR}/appServer_wrapper.log &
APPSERVER_WRAPPER_PID=$!
EXECUTE_PID="${EXECUTE_PID} ${APPSERVER_WRAPPER_PID}"
if [ $? != 0 ]; then
	log_error "execute ${APPSERVER_WRAPPER}"
	kill_wrapper
	exit 1
fi

# wait for all appServer to become active
log_info "wait for all appServer to become active ...."
while true
do
	sleep 10
	kill -0 ${APPSERVER_WRAPPER_PID} >/dev/null 2>&1
	if [ $? -ne 0 ] ; then
		log_error "execute ${APPSERVER_WRAPPER}"
		kill_wrapper
		exit 1
	fi
	# is active
	IS_ACTIVE=`grep 'The app server is active' ${APPSERVER_CLIENT_OUTPUT_DIR}/appServer_wrapper.log | wc -l`
	if [ $? != 0 ] ; then
		log_error "get ${APPSERVER_CLIENT_OUTPUT_DIR}/appServer_wrapper.log"
	fi
	if [ ${IS_ACTIVE} -ge 1 ] ; then
		log_info "all appServer is active !"
		break;
	fi
done

# run all dbdriver
echo ""
log_info "######## START ${DBDRIVER_WRAPPER} ########"
log_info "${TOOL_ROOT}/${DBDRIVER_WRAPPER} -c ${DB_CONNECTION} -e ${EU} -i ${ITEM} -u ${CUSTOMER} -t ${THINK_TIME} -d ${REAL_DURATION} -p ${PRETEST_DURATION} -f ${CONF_DIR} -o ${DBDRIVER_CLIENT_OUTPUT_DIR} 2>&1 | tee ${DBDRIVER_CLIENT_OUTPUT_DIR}/dbdriver_wrapper.log"

${TOOL_ROOT}/${DBDRIVER_WRAPPER} -c ${DB_CONNECTION} -e ${EU} -i ${ITEM} -u ${CUSTOMER} -t ${THINK_TIME} -d ${REAL_DURATION} -p ${PRETEST_DURATION} -f ${CONF_DIR} -o ${DBDRIVER_CLIENT_OUTPUT_DIR} 2>&1 | tee ${DBDRIVER_CLIENT_OUTPUT_DIR}/dbdriver_wrapper.log
EXECUTE_PID="${EXECUTE_PID} $!"
if [ $? != 0 ]; then
	log_error "execute ${DBDRIVER_WRAPPER}"
	kill_wrapper
	exit 1
fi

echo ""
kill_wrapper
exit 0
