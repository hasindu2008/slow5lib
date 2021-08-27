#!/bin/sh

set -x
set -e

gcc -Wall -O2 -I include/ examples/sequential_read.c lib/libslow5.a  -o examples/sequential_read -lm -lz
gcc -Wall -O2 -I include/ examples/random_read.c lib/libslow5.a  -o examples/random_read -lm -lz
gcc -Wall -O2 -I include/ examples/auxiliary_field.c lib/libslow5.a  -o examples/auxiliary_field -lm -lz
gcc -Wall -O2 -I include/ examples/header_attribute.c lib/libslow5.a  -o examples/header_attribute -lm -lz
gcc -Wall -O2 -I include/ examples/random_read_pthreads.c lib/libslow5.a  -o examples/random_read_pthreads -lm -lz -lpthread
gcc -Wall -O2 -I include/ examples/random_read_openmp.c lib/libslow5.a  -o examples/random_read_openmp -lm -lz -fopenmp  || echo "openmp compilation failed."
