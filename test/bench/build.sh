#!/bin/sh

set -x
set -e

make clean && make -j zstd=1 slow5_mt=1
gcc -Wall -O2 -g -I include/ -o test/bench/convert_to_pa test/bench/convert_to_pa.c lib/libslow5.a -lm -lz -lzstd -fopenmp
gcc -Wall -O2 -g -I include/ -o test/bench/convert_to_pa_rand test/bench/convert_to_pa_rand.c lib/libslow5.a -lm -lz -lzstd -fopenmp
gcc -Wall -O2 -g -I include/ -o test/bench/get_all test/bench/get_all.c lib/libslow5.a -lm -lz -lzstd -lpthread
gcc -Wall -O2 -g -I include/ -o test/bench/get_read_ids test/bench/get_read_ids.c lib/libslow5.a -lm -lz -lzstd
gcc -Wall -O2 -g -I include/ -o test/bench/get_samples test/bench/get_samples.c lib/libslow5.a -lm -lz -lzstd -lpthread  -fopenmp
gcc -Wall -O2 -g -I include/ -o test/bench/get_samples_no_pipelining test/bench/get_samples_no_pipelining.c lib/libslow5.a -lm -lz -lzstd -lpthread  -fopenmp
gcc -Wall -O2 -g -I include/ -o test/bench/get_samples_rand test/bench/get_samples_rand.c lib/libslow5.a -lm -lz -lzstd -lpthread  -fopenmp
gcc -Wall -O2 -g -I include/ -o test/bench/get_sample_count_rand test/bench/get_sample_count_rand.c lib/libslow5.a -lm -lz -lzstd -lpthread
gcc -Wall -O2 -g -I include/ -o test/bench/get_read_number_rand test/bench/get_read_number_rand.c lib/libslow5.a -lm -lz -lzstd -lpthread
