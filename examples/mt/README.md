# slow5lib mt Examples

** WARNING: This API is under development. There could be bugs **/

** This is not meant to be used by a programmer who has the expertise to write efficient multi-threaded code and use the slow5 low-level API directly. Such advanced examples are at [advanced examples](../adv/) **

This directory contains examples that uses the built in multi-threaded API in slow5lib.  For these examples to work you must build slow5lib with multi-threaded API as `make slow5_mt=1`. To link against slow5lib built with multi-threaded API, make sure to append `-lpthread` flag.

- *mt.c* demonstrates how to use multi-threaded API to first write and then fetch a batch of slow5/blow5 records in parallel.
- *lazymt.c* demonstrates how to use the lazy function to do the same as above, which is meant for a lazy programmer.

You can invoke [build.sh](build.sh) from slow5lib directory as `examples/mt/build.sh` to compile the example programmes. Have a look at the script to see the commands used for compiling and linking. Also make sure you get familiar with the basic examples first, before trying these.

