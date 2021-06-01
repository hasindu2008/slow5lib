#!/bin/bash

gcc -Wall -O2 -I include/ examples/sequential_read.c lib/libslow5.a  -o examples/sequential_read -lz
gcc -Wall -O2 -I include/ examples/random_read.c lib/libslow5.a  -o examples/random_read -lz
#gcc -Wall -O2 -I include/ -L lib/ examples/sequential_read.c   -o examples/sequential_read -lslow5 -lz
