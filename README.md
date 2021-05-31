# slow5lib

Slow5lib is a sofware library for reading & writing SLOW5 files.

Slow5lib is designed to facilitate use of data in SLOW5 format by third-party software packages. Exisiting packages that read/write data in FAST5 format can be easily modified to use SLOW5 simply by replacing the existing FAST5 API with slow5lib.

About SLOW5:

SLOW5 is a new file format for storting signal data from Oxford Nanopore Technologies (ONT) devices. SLOW5 was developed to overcome inherent limitations in the standard FAST5 signal data format that prevent efficient, scalable analysis and cause many headaches for developers.

SLOW5 is a simple tab-separated values (TSV) file encoding metadata and time-series signal data for one nanopore read per line, with global metadata stored in a file header. Parallel file access is facilitated by an accompanying index file, also in TSV format, that specifies the position of each read (in Bytes) within the main SLOW5 file. SLOW5 can be encoded in human-readable ASCII format, or a more compact and efficient binary format (BLOW5) - this is analogous to the seminal SAM/BAM format for storing DNA sequence alignments. The BLOW5 binary format can be compressed using standard gzip compression, or other compression methods, thereby minimising the data storage footprint while still permitting efficient parallel access.

Detailed benchmarking experiments have shown that SLOW5 format is up to X-fold faster and X% smaller than FAST5 [SLOW5 paper].

<todo>

<!--- [![Build Status](https://travis-ci.com/hasindu2008/slow5.svg?token=pN7xnsxgLrRxbAn8WLVQ&branch=master)](https://travis-ci.com/hasindu2008/slow5) -->
<!---[![SLOW5 C/C++ CI Local](https://github.com/hasindu2008/slow5lib/workflows/SLOW5%20C/C++%20CI%20Local/badge.svg)](https://github.com/hasindu2008/slow5lib/actions?query=workflow%3A%22SLOW5+C%2FC%2B%2B+CI+Local%22)
[![SLOW5 C/C++ CI Local OSX](https://github.com/hasindu2008/slow5lib/workflows/SLOW5%20C/C++%20CI%20Local%20OSX/badge.svg)](https://github.com/hasindu2008/slow5lib/actions/workflows/c-cpp-selfhosted-mac.yml?query=workflow%3A%22SLOW5+C%2FC%2B%2B+CI+Local+OSX%22)-->
[![SLOW5 C/C++ CI Github](https://github.com/hasindu2008/slow5lib/workflows/SLOW5%20C/C++%20CI%20Github/badge.svg)](https://github.com/hasindu2008/slow5lib/actions?query=workflow%3A%22SLOW5+C%2FC%2B%2B+CI+Github%22)

## Quick start

If you are a Linux user and want to quickly try out download the compiled binaries from the [latest release](https://github.com/hasindu2008/slow5lib/releases). For example:
```sh
VERSION=v0.2-beta
wget "https://github.com/hasindu2008/f5c/releases/download/$VERSION/slow5lib-$VERSION-binaries.tar.gz" && tar xvf slow5lib-$VERSION-binaries.tar.gz && cd slow5lib-$VERSION/
./slow5lib_x86_64_linux
```
Binaries should work on most Linux distributions and the only dependency is `zlib` which is available by default on most distros.

## Building

Users are recommended to build from the  [latest release](https://github.com/hasindu2008/slow5lib/releases) tar ball. Quick example for Ubuntu :
```sh
sudo apt-get install zlib1g-dev   #install HDF5 and zlib development libraries
VERSION=v0.2-beta
wget "https://github.com/hasindu2008/slow5lib/releases/download/$VERSION/slow5lib-$VERSION-release.tar.gz" && tar xvf slow5lib-$VERSION-release.tar.gz && cd slow5lib-$VERSION/
./configure
make
```
The commands to zlib __development libraries__ on some popular distributions :
```sh
On Debian/Ubuntu : sudo apt-get install zlib1g-dev
On Fedora/CentOS : sudo dnf/yum install zlib-devel
On OS X : brew install zlib
```


## Usage


### Examples



## Acknowledgement
Some code snippets have been taken from [Minimap2](https://github.com/lh3/minimap2) and [Samtools](http://samtools.sourceforge.net/).
