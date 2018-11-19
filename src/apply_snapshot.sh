#!/usr/bin/env sh
if [ "$#" -ne 1 ]; then
	echo "Usage : ./command <size in MB>"
	exit 1
fi

sizeMB=$1
dd if=dump.snap of=/dev/pmem1 bs=1M count=$sizeMB
