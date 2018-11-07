#!/usr/bin/env sh
dd if=/dev/pmem0 of=dump.snap bs=1M count=512
