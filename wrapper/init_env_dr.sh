#!/bin/sh

# PostgreSQL root user name
PGUSER=pgsql; export PGUSER
PGUSERHOME=`eval echo ~${PGUSER}`

# DBT-1 expand directory path
DBT1ROOT=${PGUSERHOME}/dbt1; export DBT1ROOT

# Locale
LANG=C; export LANG
