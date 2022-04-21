#!/bin/bash


make clean
rm -rf *.so python/pyslow5.cpp build/lib.* build/temp.*
GCC_ASAN_PRELOAD=$(gcc -print-file-name=libasan.so)
CFLAGS="-fsanitize=address -fno-omit-frame-pointer" python3 setup.py build
cp build/lib.*/*.so  ./
echo $GCC_ASAN_PRELOAD
LD_PRELOAD=$GCC_ASAN_PRELOAD  python3 < python/example.py
# LD_PRELOAD=$GCC_ASAN_PRELOAD  python3 -m unittest -v python/test.py
