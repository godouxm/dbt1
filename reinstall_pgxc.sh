#!/bin/sh
make clean
autoconf
autoheader
./configure --with-postgresql=/usr/local/pgsql
make
make install
