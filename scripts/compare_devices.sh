#!/usr/bin/env sh
# First allow pmem1 to complete recovery
echo "Mounting devices"
mount -o dax /dev/pmem0 /mnt/pmem0
mount -o dax /dev/pmem1 /mnt/pmem1
#echo "Unmount pmem1"
#umount /mnt/pmem1
(! cmp /dev/pmem0 /dev/pmem1) || echo "match"

diff -qr /mnt/pmem0 /mnt/pmem1 
error=$?

if [ $error -eq 0 ]; then
	echo "The files in two devices match"
elif [ $error -eq 1 ]; then
   	echo "Files in the devices differ"
else
   	echo "There was something wrong with the diff command"
fi

echo "Unmount devices"
umount /mnt/pmem0
umount /mnt/pmem1
