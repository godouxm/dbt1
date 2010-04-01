#!/bin/sh

# PostgreSQL install directory path
PGINSTALL=/usr/local/pgsql; export PGINSTALL
PGLIB=${PGINSTALL}/lib; export PGLIB
PGSHARE=/usr/local/pgsql/share; export PGSHARE
PATH=${PATH}:${PGINSTALL}/bin; export PATH
LD_LIBRARY_PATH=/lib:${PGLIB}; export LD_LIBRARY_PATH

# PostgreSQL root user name
PGUSER=pgsql; export PGUSER
PGUSERHOME=`eval echo ~${PGUSER}`
PGPORT=5432; export PGPORT

# PostgreSQL dababase cluster path
PGDATA=/array1/data; export PGDATA
ARCHIVELOG=/array3/archive_log
PGXLOG=/array2/pg_xlog; export PGXLOG

# DBT-1 expand directory path
DBT1ROOT=${PGUSERHOME}/dbt1; export DBT1ROOT

# DBT-1 Database Name
DBNAME=DBT1; export DBNAME

# Locale
LANG=C; export LANG
