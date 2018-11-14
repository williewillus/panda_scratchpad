#!/bin/sh

file1="/mnt/pmem0/foo"
file2="/mnt/pmem0/bar"

touch $file1
echo 1 > $file1
touch $file2
echo "hello" > $file2
