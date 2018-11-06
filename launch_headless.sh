#!/bin/sh

if [ "$#" -ne 2 ]; then
	echo "Usage : ./launch_headless <path_to_img_file> <memory M|G>"
	exit 1
fi

drive_path=$1
memory=$2

./launch.sh $drive_path $memory '-nographic'
