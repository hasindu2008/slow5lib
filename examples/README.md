# slow5lib Examples

This directory contains following examples.
- *sequential_read.c* demonstrates how to read a slow5/blow5 file, sequentially from start to end.
- *random_read.c* demonstrates how to fetch a given read ID from a slow5/blow5 file.
- *header_attribute.c* demonstrates how to fetch a header data attribute from a slow5/blow5 file.
- *auxiliary_field.c* demonstrates how to fetch a auxiliary field from a slow5/blow5 file.
- *random_read_pthreads.c* demonstrates how to fetch given read IDs in parallel from a slow5/blow5 file using *pthreads*.
- *random_read_openmp.c* demonstrates how to fetch given read IDs in parallel from a slow5/blow5 file using openMP.
- *write.c* demonstrate how to write a new slow5/blow5 file.
- *append.c* demonstrates how to append to an existing slow5/blow5 file.

You can invoke [build.sh](build.sh) from slow5lib directory as `examples/build.sh` to compile the example programmes. Have a look at the script to see the commands used for compiling and linking. As an example, the command to compile [sequential_read.c](sequential_read.c) is `gcc -Wall -O2 -I include/ examples/sequential_read.c lib/libslow5.a  -o examples/sequential_read -lm -lz`. Make sure that you first call `make` so that `lib/libslow5.a` becomes available to be linked with. If you compiled *slow5lib* with *zstd* support enabled (`make zstd=1`), make sure you append `-lzstd` to the compilation commands.

A public template repository is available at [https://github.com/hasindu2008/slow5-template] which you can directly use to setup your own repository that uses *slow5lib* to build a tool. Check the instructions and comments there.