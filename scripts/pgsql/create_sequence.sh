#!/bin/bash
#
if [ $# -lt 1 ]; then
	echo "customer numbers are required."
	echo "Usage: create_sequence.sh <customer numbers>"
	exit 1
fi
CUSTOMER=$1
echo "generate sequence. customer : ${CUSTOMER}"

END_VAL=`expr 2880 \* ${CUSTOMER} + 1`
psql -d $SID1 -c "CREATE SEQUENCE custid INCREMENT 1 START ${END_VAL};COMMIT;"

END_VAL=`expr ${CUSTOMER} \* 2880 \* 2 + 1`
psql -d $SID1 -c "CREATE SEQUENCE addrid INCREMENT 1 START ${END_VAL};COMMIT;"

END_VAL=`expr ${CUSTOMER} \* 2880 \* 10 / 9 + 1`
psql -d $SID1 -c "CREATE SEQUENCE scid INCREMENT 1 START ${END_VAL};COMMIT;"

