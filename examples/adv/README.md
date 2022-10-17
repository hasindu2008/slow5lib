# slow5lib Advanced Examples

This directory contains following advanced examples that uses low-level API.
- *auxiliary_field_enum.c* demonstrates how to fetch a auxiliary field of enum data type from a slow5/blow5 file.
- *sequential_read_pthreads.c* demonstrates how to sequentially read raw SLOW5 records from a slow5/blow5 file using a single thread and then decode those in parallel using *pthreads*.
- *sequential_read_openmp.c* demonstrates how to sequentially read raw SLOW5 records from a slow5/blow5 file using a single thread and then decode those in parallel using *openMP*.
- *get_all_read_ids.c* demonstrates how to get the list of all read IDs from a slow5/blow5 file.
- *auxiliary_field_enum_write.c* demonstrates how to write a slow5/blow5 file containing an auxiliary field of type enum.

You can invoke [build.sh](build.sh) from slow5lib directory as `examples/adv/build.sh` to compile the example programmes. Have a look at the script to see the commands used for compiling and linking. Also make sure you get familiar with the basic examples first, before trying these advanced examples.

