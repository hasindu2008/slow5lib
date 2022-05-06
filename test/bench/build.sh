#!/bin/sh

set -x
set -e

gcc -Wall -O2 -I include/ -o get_all_read_ids test/bench/get_all_read_ids.c lib/libslow5.a -lm -lz -lzstd
