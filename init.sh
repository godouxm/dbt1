#!/bin/bash

# developer: customize this file with your local setup. The following commands
# will generate configure script for you from configure.in. And make dist will
# make a distribution tarball for you to test.

aclocal || exit
autoheader || exit
autoconf || exit

exit 0

