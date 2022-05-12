#!/bin/sh

set -x
set -e

gcc -Wall -O2 -g -I include/ -o get_all_read_ids test/bench/get_all_read_ids.c lib/libslow5.a -lm -lz -lzstd
gcc -Wall -O2 -g -I include/ -o get_all_samples test/bench/get_all_samples.c lib/libslow5.a python/slow5threads.c -lm -lz -lzstd -lpthread  -fopenmp
gcc -Wall -O2 -g -I include/ -o get_selected_read_ids_samples test/bench/get_selected_read_ids_samples.c lib/libslow5.a python/slow5threads.c -lm -lz -lzstd -lpthread  -fopenmp
gcc -Wall -O2 -g -I include/ -o get_selected_read_ids_sample_count test/bench/get_selected_read_ids_sample_count.c lib/libslow5.a python/slow5threads.c -lm -lz -lzstd -lpthread
gcc -Wall -O2 -g -I include/ -o get_selected_read_ids_read_number test/bench/get_selected_read_ids_read_number.c lib/libslow5.a python/slow5threads.c -lm -lz -lzstd -lpthread
