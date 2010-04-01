
######################################################
#	log writer
#	arg1: log level (ERROR,WARNING,INFO)
#	arg2: output message
######################################################
logWriter()
{
	local logLevel="$1"
	local logMsg="$2"

	now=`date +'%Y-%m-%d %H:%M:%S'`
	echo "${now} [${logLevel}] : ${logMsg}"
	return 0
}

######################################################
#	output log (info)
#	arg1: output message
######################################################
log_info()
{
	local logMsg="$1"
	logWriter "INFO" "${logMsg}"
	return 0
}

######################################################
#	output log (warning)
#	arg1: output message
######################################################
log_warn()
{
	local logMsg="$1"
	logWriter "WARNING" "${logMsg}"
	return 0
}

######################################################
#	output log (error)
#	arg1: output message
######################################################
log_error()
{
	local logMsg="$1"
	logWriter "ERROR" "${logMsg}"
	return 0
}

######################################################
#	Send SIGTERM to specified script
#	arg1: script name
#	arg2: host name
#	arg2: user name
######################################################
function kill_script()
{
	local SH_NAME=$1
	local RUN_HOST=$2
	local RUN_USER=$3
	local SH_PID

	SH_PID=`ssh ${RUN_USER}@${RUN_HOST} "/sbin/pidof ${SH_NAME}"`
	if [ $? != 0 ] ; then
		log_warn "can't get PID of ${SH_NAME} (${RUN_USER}@${RUN_HOST})"
		return 1
	fi
	if [ "${SH_PID}" != "" ] ; then
		log_info "kill ${RUN_USER}@${RUN_HOST} ${SH_NAME} (${SH_PID})"
		ssh ${RUN_HOST} "kill -s TERM ${SH_PID} >/dev/null 2>&1"
	fi
	return 0
}

######################################################
#	Send SIGTERM to specified script
#	arg1: script name
#	arg2: user name
#	global arg EXECUTE_RUN_HOST : host name list ('host1 host2 ...')
#	global arg STAT_TOOL : stat tool
#	global arg CPU_STAT_TOOL : cpu stat tool
#	global arg IO_STAT_TOOL : io stat tool
######################################################
function kill_script_all()
{
	local SH_NAME=$1
	local RUN_USER=$2
	local RUN_HOST

	log_info "kill all ${SH_NAME} process ...."
	for RUN_HOST in ${EXECUTE_RUN_HOST}
	do
		kill_script ${SH_NAME} ${RUN_HOST} ${RUN_USER}
		kill_script ${STAT_TOOL} ${RUN_HOST} ${RUN_USER}
		kill_script ${CPU_STAT_TOOL} ${RUN_HOST} ${RUN_USER}
		kill_script ${IO_STAT_TOOL} ${RUN_HOST} ${RUN_USER}
	done
	return 0
}

