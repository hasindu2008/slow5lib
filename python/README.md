# pyslow5 python library

The slow5 python library (pyslow5) allows a user to read slow5 and blow5 files.

## Installation

Initial setup and example info for environment
###### slow5lib needs python3.4.2 or higher.
```bash
# If your native python3 meets this requirement, you can use that, or use a
# specific version installed with deadsnakes below. If you install with deadsnakes,
# you will need to call that specific python, such as python3.8 or python3.9,
# in all the following commands until you create a virtual environment with venv.
# Then once activated, you can just use python3.

# To install a specific version of python, the deadsnakes ppa is a good place to start
# This is an example for installing python3.7
# you can then call that specific python version
# > python3.7 -m pip --version
sudo add-apt-repository ppa:deadsnakes/ppa
sudo apt-get update
sudo apt install python3.7 python3.7-dev python3.7-venv


# get zlib1g-dev
sudo apt-get update && sudo apt-get install -y zlib1g-dev

# Chekc with
python3 --version

# You will also need the python headers if you don't already have them installed.

sudo apt-get install python3-dev
```

Building and installing the python library.

```bash
python3 -m venv /path/to/slow5libvenv
source /path/to/slow5libvenv/bin/activate
python3 -m pip install --upgrade pip
python3 -m pip install setuptools cython numpy wheel

git clone git@github.com:hasindu2008/slow5lib.git
cd slow5lib
make

# CHOOSE A OR B:
# |=======================================================================|
# |A. Install with pip if wheel is present, otherwise it uses setuptools  |
    python3 -m pip install . --use-feature=in-tree-build
# |=======================================================================|
# |B. Or build and install manually with setup.py                         |
# |build the package                                                      |
    python3 setup.py build
# |If all went well, install the package                                  |
    python3 setup.py install
# |=======================================================================|

# This should not require sudo if using a python virtual environment/venv
# confirm installation, and find pyslow5==<version>
python3 -m pip freeze

# Ensure slow5 library is working by running the basic tests
python3 ./python/example.py


# To Remove the library
python3 -m pip uninstall pyslow5
```

## Usage

#### `Open(FILE, mode, DEBUG=0)`:

The pyslow5 libraryr has one main Class, `pyslow5.Open` which opens a slow5/blow5 (slow5 for easy reference) file for reading.

`FILE`: the file or filepath of the slow5 file to open
`mode`: mode in which to open the file. Currently, only `r` is accepted for read only.

This is designed to mimic Python's native Open() to help users remember the syntax

Example:

```python
import pyslow5

# open file
s5 = pyslow5.Open('examples/example.slow5','r')
```

When opening a slow5 file for the first time, and index will be created and saved in the same directory as the file being read. This index will then be loaded. For files that already have an index, that index will be loaded.

#### `seq_reads(pA=False, aux=None)`:

Access all reads sequentially in an opened slow5.
+ If readID is not found, `None` is returned.
+ pA = Bool for converting signal to picoamps.
+ aux = `str` '<attr_name>'/'all' or list of names of auxiliary fields added to return dictionary, `None` if `<attr_name>` not found
+ returns `dict` = dictionary of main fields for read_id, with any aux fields added

Example:

```python
# create generator
reads = s5.seq_reads()

# print all readIDs
for read in reads:
    print(read['read_id'])

# or use directly in a for loop
for read in s5.seq_reads(pA=True, aux='all'):
    print("read_id:", read['read_id'])
    print("read_group:", read['read_group'])
    print("digitisation:", read['digitisation'])
    print("offset:", read['offset'])
    print("range:", read['range'])
    print("sampling_rate:", read['sampling_rate'])
    print("len_raw_signal:", read['len_raw_signal'])
    print("signal:", read['signal'][:10])
    print("================================")
```

#### `get_read(readID, pA=False, aux=None)`:

Access a specific read using a unique readID. This is a ranom access method, using the index.
+ If readID is not found, `None` is returned.
+ pA = Bool for converting signal to picoamps.
+ aux = `str` '<attr_name>'/'all' or list of names of auxiliary fields added to return dictionary, `None` if `<attr_name>` not found
+ returns `dict` = dictionary of main fields for read_id, with any aux fields added

Example:

```python
readID = "r1"
read = s5.get_read(readID, pA=True, aux=["read_number", "start_mux"])
if read is not None:
    print("read_id:", read['read_id'])
    print("len_raw_signal:", read['len_raw_signal'])
```


#### `get_read_list(read_list, pA=False, aux=None)`:

Access a list of specific reads using a list `read_list` of unique readIDs. This is a random access method using the index, so order of readIDs does impact access speed.
+ If readID is not found, `None` is returned.
+ pA = Bool for converting signal to picoamps.
+ aux = `str` '<attr_name>'/'all' or list of names of auxiliary fields added to return dictionary, `None` if `<attr_name>` not found
+ returns `dict` = dictionary of main fields for read_id, with any aux fields added

Example:

```python
read_list = ["r1", "r3", "null_read", "r5", "r2", "r1"]
selected_reads = s5.get_read_list(read_list)
for r, read in zip(read_list,selected_reads):
    if read is not None:
        print(r, read['read_id'])
    else:
        print(r, "read not found")
```


#### `get_header_names()`:

Returns a list containing the uninon of header names from all read_groups

#### `get_header_value(attr, read_group=0)`:

Returns a `str` of the value of a header attribute (`attr`) for a particular read_group.
Returns `None` if value can't be found

#### `get_all_headers(read_group=0)`:

Returns a dictionary with all header attributes and values for a particular read_group
If there are values present for one read_group, and not for another, the attribute will still be returned for the read_group without, but with a value of `None`.

#### `get_aux_names()`:

Returns an ordered list of auxiliary attribute names. (same order as get_aux_types())

This is used for understanding which auxiliary attributes are available within the slow5 file, and providing selections to the `aux` keyword argument in the above functoions

#### `get_aux_types()`:

Returns an ordered list of auxiliary attribute types (same order as get_aux_names())

This can mostly be ignored, but will be used in error tracing in the future, as auxiliary field requests have multiple types, each with their own calls, and not all are used. It could be the case a call for an auxiliary filed fails, and knowing which type the field is requesting is very helpful in understanding which function in C is being called, that could be causing the error.

See documentation for full example
