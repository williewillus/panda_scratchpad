#!/usr/bin/env sh
diff /dev/pmem0 /dev/pmem1 && echo "pmem0 and pmem1 don't match!" || echo "match"
