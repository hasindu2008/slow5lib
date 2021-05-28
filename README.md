# slow5lib

A library for reading SLOW5 files.
<todo>

<!--- [![Build Status](https://travis-ci.com/hasindu2008/slow5.svg?token=pN7xnsxgLrRxbAn8WLVQ&branch=master)](https://travis-ci.com/hasindu2008/slow5) -->
[![SLOW5 C/C++ CI Local](https://github.com/hasindu2008/slow5lib/workflows/SLOW5%20C/C++%20CI%20Local/badge.svg)](https://github.com/hasindu2008/slow5lib/actions?query=workflow%3A%22SLOW5+C%2FC%2B%2B+CI+Local%22)
[![SLOW5 C/C++ CI Local OSX](https://github.com/hasindu2008/slow5lib/workflows/SLOW5%20C/C++%20CI%20Local%20OSX/badge.svg)](https://github.com/hasindu2008/slow5lib/actions/workflows/c-cpp-selfhosted-mac.yml?query=workflow%3A%22SLOW5+C%2FC%2B%2B+CI+Local+OSX%22)
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
```


## Usage


### Examples



## Acknowledgement
Some code snippets have been taken from [Minimap2](https://github.com/lh3/minimap2) and [Samtools](http://samtools.sourceforge.net/).
