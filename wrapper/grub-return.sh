#!/bin/sh

#------------------------------------------
# Check arguments
#------------------------------------------
DB_SERVER=DL580G5-04

#----------------------------------------
# Main Function
#----------------------------------------
# Return grub.conf
ssh ${DB_SERVER} "
	pushd /boot/grub > /dev/null 2>&1; \\
	if [ -e grub.conf.def ]; then \\
		if [ -e grub.conf ]; then \\
			sudo unlink grub.conf; \\
			sudo cp grub.conf.def grub.conf; \\
		else \\
			echo "--- there is no grub.conf ---"; \\
		fi; \\
	else \\
		echo "--- there is no grub.conf.def ---"; \\
	fi; \\
	popd > /dev/null 2>&1; \\
	sudo cat /boot/grub/grub.conf;
"
exit 0

