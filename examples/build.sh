#!/bin/sh

set -x
set -e

gcc -Wall -O2 -I include/ examples/sequential_read.c lib/libslow5.a  -o examples/sequential_read -lm -lz
gcc -Wall -O2 -I include/ examples/random_read.c lib/libslow5.a  -o examples/random_read -lm -lz
gcc -Wall -O2 -I include/ examples/get_aux_field.c lib/libslow5.a  -o examples/get_aux_field -lm -lz
gcc -Wall -O2 -I include/ examples/get_hdr_attribute.c lib/libslow5.a  -o examples/get_hdr_attribute -lm -lz
gcc -Wall -O2 -I include/ examples/pthread_get_reads.c lib/libslow5.a  -o examples/pthread_get_reads -lm -lz -lpthread
gcc -Wall -O2 -I include/ examples/openmp_get_reads.c lib/libslow5.a  -o examples/openmp_get_reads -lm -lz -fopenmp
