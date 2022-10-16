#!/bin/sh

#exit on error
set -x
#prints the command to the console
set -e
#make sure slow5lib is built with multi-threading API as `make slow5_mt=1`
make clean && make slow5_mt=1
gcc -Wall -O2 -I include/ examples/mt/mt.c lib/libslow5.a  -o examples/mt/mt -lm -lz -lpthread
gcc -Wall -O2 -I include/ examples/mt/lazymt.c lib/libslow5.a  -o examples/mt/lazymt -lm -lz -lpthread

#append -lzstd to above commands if your slow5lib is built with zstd support