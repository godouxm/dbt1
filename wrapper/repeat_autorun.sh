#!/bin/sh
# Usage:
# ./autorun.sh <case name> <eu> <conn num> <test duration> <pre test duration>
#
# (change number of cpu core case)
#   [change script]
#   ./grub-change.sh <change grub.conf>
#   [return script]
#   ./grub-return.sh
#
# ex.)
# ./autorun.sh test01_core16_eu6000 6000 200 3600 1800
# ./grub-change.sh grub.conf.core8
# ./autorun.sh test02_core8_eu6000 6000 200 3600 1800
# ./grub-return.sh
# ./autorun.sh test03_core16_eu7000 7000 200 3600 1800


exit 0

