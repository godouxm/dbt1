#!/bin/sh
make clean
autoconf
autoheader
./configure --with-postgresql=$HOME/pgsql
make
make install
