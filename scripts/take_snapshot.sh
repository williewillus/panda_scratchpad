#!/usr/bin/env sh
if [ "$#" -ne 1 ]; then
	echo "Usage : ./command <size in MB>"
	exit 1
fi

sizeMB=$1
dd if=/dev/pmem0 of=/mnt/ramdisk/dump.snap bs=1M count=$sizeMB
