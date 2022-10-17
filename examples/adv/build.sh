#!/bin/sh

#exit on error
set -x
#prints the command to the console
set -e
gcc -Wall -O2 -I include/ examples/adv/auxiliary_field_enum.c lib/libslow5.a  -o examples/adv/auxiliary_field_enum -lm -lz
gcc -Wall -O2 -I include/ examples/adv/sequential_read_openmp.c lib/libslow5.a  -o examples/adv/sequential_read_openmp -lm -lz -fopenmp
gcc -Wall -O2 -I include/ examples/adv/sequential_read_pthreads.c lib/libslow5.a  -o examples/adv/sequential_read_pthreads -lm -lz -lpthread
gcc -Wall -O2 -I include/ examples/adv/get_all_read_ids.c lib/libslow5.a  -o examples/adv/get_all_read_ids -lm -lz
gcc -Wall -O2 -I include/ examples/adv/auxiliary_field_enum_write.c lib/libslow5.a  -o examples/adv/auxiliary_field_enum_write -lm -lz

#append -lzstd to above commands if your slow5lib is built with zstd support