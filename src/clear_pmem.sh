#!/usr/bin/env sh
dd if=/dev/zero of=/dev/pmem0 bs=1M count=512
dd if=/dev/zero of=/dev/pmem1 bs=1M count=512
