#!/usr/bin/env sh
(! cmp /dev/pmem0 /dev/pmem1) || echo "match"
