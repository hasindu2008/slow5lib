#!/bin/sh

#exit on error
set -x
#prints the command to the console
set -e

gcc -Wall -O2 -I include/ examples/sequential_read.c lib/libslow5.a  -o examples/sequential_read -lm -lz
gcc -Wall -O2 -I include/ examples/random_read.c lib/libslow5.a  -o examples/random_read -lm -lz
gcc -Wall -O2 -I include/ examples/auxiliary_field.c lib/libslow5.a  -o examples/auxiliary_field -lm -lz
gcc -Wall -O2 -I include/ examples/header_attribute.c lib/libslow5.a  -o examples/header_attribute -lm -lz
gcc -Wall -O2 -I include/ examples/random_read_pthreads.c lib/libslow5.a  -o examples/random_read_pthreads -lm -lz -lpthread
gcc -Wall -O2 -I include/ examples/random_read_openmp.c lib/libslow5.a  -o examples/random_read_openmp -lm -lz -fopenmp  || echo "openmp compilation failed." #so that the GitHub CI does not fail for macOS
gcc -Wall -O2 -I include/ examples/write.c lib/libslow5.a  -o examples/write -lm -lz
gcc -Wall -O2 -I include/ examples/append.c lib/libslow5.a  -o examples/append -lm -lz

#append -lzstd to above commands if your slow5lib is built with zstd support