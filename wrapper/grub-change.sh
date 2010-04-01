#!/bin/sh

#------------------------------------------
# Check arguments
#------------------------------------------
GRUBFILE=$1
DB_SERVER=DL580G5-04

#----------------------------------------
# Main Function
#----------------------------------------
# Set grub.conf
ssh ${DB_SERVER} "
	pushd /boot/grub > /dev/null 2>&1; \\
	sudo chown root:root ${GRUBFILE}; \\
	if [ -e grub.conf.def ]; then \\
		if [ -e grub.conf ]; then \\
			sudo rm -f grub.conf; \\
		fi; \\
	else \\
		if [ -e grub.conf ]; then \\
			sudo cp grub.conf grub.conf.def; \\
			sudo rm -f grub.conf; \\
		fi; \\
	fi; \\
	sudo ln -s ${GRUBFILE} grub.conf; \\
	popd > /dev/null 2>&1; \\
	sudo cat /boot/grub/grub.conf;
"
exit 0

