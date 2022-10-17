#!/bin/bash

die() {
	echo "$1" >&2
	echo
	exit 1
}

make clean
rm -rf *.so python/pyslow5.cpp python/pyslow5.c build/lib.* build/temp.* asan.log
GCC_ASAN_PRELOAD=$(gcc -print-file-name=libasan.so)
CFLAGS="-fsanitize=address -fno-omit-frame-pointer" python3 setup.py build  || die "Failed to build pyslow5"
cp build/lib.*/*.so  ./ || die "Failed to copy .so file"
echo $GCC_ASAN_PRELOAD
LD_PRELOAD=$GCC_ASAN_PRELOAD  python3 < python/example.py &> asan.log
tail asan.log  || die "Failed to tail asan.log"
leaks=$(tail asan.log  | grep "SUMMARY: AddressSanitizer" | awk '{print $3}')
# if [ $leaks -gt 1607006 ]; then
# 	die "Memory leak detected"
# fi
# LD_PRELOAD=$GCC_ASAN_PRELOAD  python3 -m unittest -v python/test.py
