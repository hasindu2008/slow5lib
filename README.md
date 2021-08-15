# slow5lib

Slow5lib is a software library for reading & writing SLOW5 files.

Slow5lib is designed to facilitate use of data in SLOW5 format by third-party software packages. Existing packages that read/write data in FAST5 format can be easily modified to use SLOW5 simply by replacing the existing FAST5 API with slow5lib.

About SLOW5:

SLOW5 is a new file format for storing signal data from Oxford Nanopore Technologies (ONT) devices. SLOW5 was developed to overcome inherent limitations in the standard FAST5 signal data format that prevent efficient, scalable analysis and cause many headaches for developers.

SLOW5 is a simple tab-separated values (TSV) file encoding metadata and time-series signal data for one nanopore read per line, with global metadata stored in a file header. Parallel file access is facilitated by an accompanying index file, also in TSV format, that specifies the position of each read (in Bytes) within the main SLOW5 file. SLOW5 can be encoded in human-readable ASCII format, or a more compact and efficient binary format (BLOW5) - this is analogous to the seminal SAM/BAM format for storing DNA sequence alignments. The BLOW5 binary format can be compressed using *zlib* (DEFLATE) compression, or other compression methods, thereby minimising the data storage footprint while still permitting efficient parallel access.

Detailed benchmarking experiments have shown that SLOW5 format is an order of magnitude faster and 25% smaller than FAST5.


[![SLOW5 C/C++ CI Github](https://github.com/hasindu2008/slow5lib/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/hasindu2008/slow5lib/actions/workflows/c-cpp.yml)
[![SLOW5 Python CI Github](https://github.com/hasindu2008/slow5lib/actions/workflows/python.yml/badge.svg)](https://github.com/hasindu2008/slow5lib/actions/workflows/python.yml)

Full documentation: https://hasindu2008.github.io/slow5lib  
Pre-print: https://www.biorxiv.org/content/10.1101/2021.06.29.450255v1

## Building

To build the C/C++ library :

```sh
sudo apt-get install zlib1g-dev   #install zlib development libraries
git clone https://github.com/hasindu2008/slow5lib
make
```

This will generate *lib/libslow5.a* for static linking and *libslow5.so* for dynamic linking.

The commands to zlib __development libraries__ on some popular distributions :
```sh
On Debian/Ubuntu : sudo apt-get install zlib1g-dev
On Fedora/CentOS : sudo dnf/yum install zlib-devel
On OS X : brew install zlib
```

To build the Python wrapper see the instructions [here](https://hasindu2008.github.io/slow5lib/pyslow5_api/pyslow5.html).

## Usage

Simply include `<slow5/slow5.h>` in your C program and call the API functions. To compile your program and statically link against slow5lib:

```
gcc [OPTIONS] -I path/to/slow5lib/include your_program.c path/to/slow5lib/lib/libslow5.a -lm -lz
```
*path/to/slow5lib/* is the absolute or relative path to the *slow5lib* repository cloned above.


To dynamically link:
```
gcc [OPTIONS] -I path/to/slow5lib/include your_program.c -L path/to/slow5lib/lib/ -lslow5 -lm -lz
```

For the documentation of the C API visit [here](https://hasindu2008.github.io/slow5lib/slow5_api/slow5.html). and for the Python API visit [here](https://hasindu2008.github.io/slow5lib/pyslow5_api/pyslow5.html).


### Examples

Examples are provided under [examples](https://github.com/hasindu2008/slow5lib/tree/master/examples).

*sequential_read.c* demonstrates how to read a slow5/blow5 file, sequentially from start to end.
*random_read.c* demonstrates how to fetch a given read ID from a slow5/blow5 file.
*header_attribute.c* demonstrates how to fetch a header data attribute from a slow5/blow5 file.
*auxiliary_field.c* demonstrates how to fetch a auxiliary field from aslow5/blow5 file.  

*random_read_pthreads.c* demonstrates how to fetch given read IDs in parallel from a slow5/blow5 file using *pthreads*.
*random_read_openmp.c* demonstrates how to fetch given read IDs in parallel from a slow5/blow5 file using openMP.

You can invoke `examples/build.sh` to compile the example programmes. Have a look at the script to see the commands used for compiling and linking.



## Acknowledgement
slow5lib uses [klib](https://github.com/attractivechaos/klib). Some code snippets have been taken from [Minimap2](https://github.com/lh3/minimap2) and [Samtools](http://samtools.sourceforge.net/).
