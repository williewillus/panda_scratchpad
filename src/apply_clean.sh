#!/usr/bin/env sh
dd if=dump.snap of=/dev/pmem1 bs=1M count=512
