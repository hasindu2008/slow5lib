# pyslow5 python library

The slow5 python library (pyslow5) allows a user to read and write slow5/blow5 files.

## Installation

Initial setup and example info for environment

###### slow5lib needs python3.4.2 or higher.

If you only want to use the python library, then you can simply install using pip

Using a virtual environment (see below if you need to install python)

#### Optional zstd compression

You can optionally enable [*zstd* compression](https://facebook.github.io/zstd) support when building *slow5lib/pyslow5*. This requires __zstd 1.3 or higher development libraries__ installed on your system:

```sh
On Debian/Ubuntu : sudo apt-get libzstd1-dev
On Fedora/CentOS : sudo yum libzstd-devel
On OS X : brew install zstd
```

BLOW5 files compressed with *zstd* offer smaller file size and better performance compared to the default *zlib*. However, *zlib* runtime library is available by default on almost all distributions unlike *zstd* and thus files compressed with *zlib* will be more 'portable'.

```bash
python3 -m venv path/to/slow5libvenv
source path/to/slow5libvenv/bin/activate
python3 -m pip install --upgrade pip
python3 -m pip install setuptools cython numpy wheel
# do this separately, after the libs above
# zlib only build
python3 -m pip install pyslow5

# for zstd build, run the following
export PYSLOW5_ZSTD=1
python3 -m pip install pyslow5
```

### Dev install

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

# Check with
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
# (B is the cleanest method)
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

### Reading/writing a file

#### `Open(FILE, mode, rec_press="zlib", sig_press="svb_zd", DEBUG=0)`:

The pyslow5 library has one main Class, `pyslow5.Open` which opens a slow5/blow5 (slow5 for easy reference) file for reading/writing.

`FILE`: the file or filepath of the slow5 file to open
`mode`: mode in which to open the file.
+ `r`= read only
+ `w`= write/overwrite
+ `a`= append

This is designed to mimic Python's native Open() to help users remember the syntax

To set the record and signal compression methods, use the following `rec_press` and `sig_press` optional args, however these are only used with `mode='w'`. Any append will use whatever is already set in the file.

Compression Options:

`rec_press`:
- "none"
- "zlib" [default]
- "zstd" [requires `export PYSLOW5_ZSTD=1` when building]

`sig_press`:
- "none"
- "svb_zd" [default]

Example:

```python
import pyslow5

# open file
s5 = pyslow5.Open('examples/example.slow5','r')
```

When opening a slow5 file for the first time, and index will be created and saved in the same directory as the file being read. This index will then be loaded. For files that already have an index, that index will be loaded.

#### `get_read_ids()`:

returns a list and total number of reads from the index.
If there is no index, it creates one first.

Example:

```python
read_ids, num_reads = s5.get_read_ids()

print(read_ids)
print("number of reads: {}".format(num_reads))
```

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


#### `seq_reads_multi(threads=4, batchsize=4096, pA=False, aux=None)`:

Access all reads sequentially in an opened slow5, using multiple threads.
+ If readID is not found, `None` is returned.
+ threads = number of threads to use in C backend.
+ batchsize = number of reads to fetch at a time. Higher numbers use more ram, but is more efficient with more threads.
+ pA = Bool for converting signal to picoamps.
+ aux = `str` '<attr_name>'/'all' or list of names of auxiliary fields added to return dictionary, `None` if `<attr_name>` not found
+ returns `dict` = dictionary of main fields for read_id, with any aux fields added

Example:

```python
# create generator
reads = s5.seq_reads_multi(threads=2, batchsize=3)

# print all readIDs
for read in reads:
    print(read['read_id'])

# or use directly in a for loop
for read in s5.seq_reads_multi(threads=2, batchsize=3, pA=True, aux='all'):
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

Access a list of specific reads using a list `read_list` of unique readIDs. This is a random access method using the index. If an index does not exist, it will create one first.
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


#### `get_read_list_multi(read_list, threads=4, batchsize=100, pA=False, aux=None):`:

Access a list of specific reads using a list `read_list` of unique readIDs using multiple threads. This is a random access method using the index. If an index does not exist, it will create one first.
+ If readID is not found, `None` is returned.
+ threads = number of threads to use in C backend
+ batchsize = number of reads to fetch at a time. Higher numbers use more ram, but is more efficient with more threads.
+ pA = Bool for converting signal to picoamps.
+ aux = `str` '<attr_name>'/'all' or list of names of auxiliary fields added to return dictionary, `None` if `<attr_name>` not found
+ returns `dict` = dictionary of main fields for read_id, with any aux fields added
Example:

```python
read_list = ["r1", "r3", "null_read", "r5", "r2", "r1"]
selected_reads = s5.get_read_list_multi(read_list, threads=2, batchsize=3)
for r, read in zip(read_list, selected_reads):
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

#### `get_aux_enum_labels(label)`:

Returns an ordered list representing the values in the enum struct in the type header.

The value in the read can then be used to access the labels as an index to the list.

Example:

```python
s5 = slow5.Open(file,'w')
end_reason_labels = s5.get_aux_enum_labels('end_reason')
print(end_reason_labels)

> ['unknown', 'partial', 'mux_change', 'unblock_mux_change', 'signal_positive', 'signal_negative']

readID = "r1"
read = s5.get_read(readID, aux='all')
er_index = read['end_reason']
er = end_reason_labels[er_index]

print("{}: {}".format(er_index, er))

> 4: signal_positive
```

### Writing a file

To write a file, `mode` in `Open()` must be set to `'w'` and when appending, `'a'`

#### `get_empty_header(aux=False)`:

Returns a dictionary containing all known header attributes with their values set to `None`.

User can modify each value, and add or remove attributes to be used has header items.
All values end up stored as strings, and anything left as `None` will be skipped.
To write header, see `write_header()`

If `aux=True`, an ordered list of strings for the enum `end_reason` will be returned.
This can be modified depending on the end reason.

Example:

```python
s5 = slow5.Open(file,'w')
header = s5.get_empty_header()
```

`end_reason` enum example

```python
s5 = slow5.Open(file, w)
header, end_reason_labels = s5.get_empty_header(aux=True)
```

#### `write_header(header, read_group=0, end_reason_labels=None)`:

Write header to file

+ `header` = populated dictionary from `get_empty_header()`
+ read_group = read group integer for when multiple runs are written to the same slow5 file
+ end_reason_labels = ordered list used for end_reason enum
+ returns 0 on success, <0 on error with error code

You must write `read_group=0` (default) first before writing any other read_groups, and it is advised to write read_groups in sequential order.

Example:

```python
# Get some empty headers
header = s5.get_empty_header()
header2 = s5.get_empty_header()

# Populate headers with some test data
counter = 0
for i in header:
    header[i] = "test_{}".format(counter)
    counter += 1

for i in header2:
    header2[i] = "test_{}".format(counter)
    counter += 1

# Write first read group
ret = s5.write_header(header)
print("ret: write_header(): {}".format(ret))
# Write second read group, etc
ret = s5.write_header(header2, read_group=1)
print("ret: write_header(): {}".format(ret))
```

`end_reason` example:

```python
# Get some empty headers
header, end_reason_labels = s5.get_empty_header(aux=True)

# Populate headers with some test data
counter = 0
for i in header:
    header[i] = "test_{}".format(counter)
    counter += 1

# Write first read group
ret = s5.write_header(header, end_reason_labels=end_reason_labels)
print("ret: write_header(): {}".format(ret))
```

#### `get_empty_record(aux=False)`:

Get empty read record for populating with data. Use with `write_record()`

+ aux = Bool for returning empty aux dictionary as well as read dictionary
+ returns a single read dictionary or a read and aux dictionary depending on aux flag

Example:
```python
# open some file to read. We will copy the data then write it
# including aux fields
s5_read = slow5.Open(read_file,'r')
reads = s5_read.seq_reads(aux='all')

# For each read in s5_read...
for read in reads:
    # get an empty record and aux dictionary
    record, aux = s5.get_empty_record(aux=True)
    # for each field in read...
    for i in read:
        # if the field is in the record dictionary...
        if i in record:
            # copy the value over...
            record[i] = read[i]
        do same for aux dictionary
        if i in aux:
            aux[i] = read[i]
    # write the record
    ret = s5.write_record(record, aux)
    print("ret: write_record(): {}".format(ret))
```

#### `write_record(record, aux=None)`:

Write a record and optional aux fields.

+ record = a populated dictionary from `get_empty_record()`
+ aux = an empty aux record returned by `get_empty_record(aux=True)`
+ returns 0 on success and -1 on error/failure

Example:

```python

record, aux = s5.get_empty_record(aux=True)
# populate record, aux dictionaries
#....
# Write record
ret = s5.write_record(record, aux)
print("ret: write_record(): {}".format(ret))
```


#### `write_record_batch(records, threads=4, batchsize=4096, aux=None)`:

Write a record and optional aux fields, using multiple threads

+ records = a dictionary of dictionaries where each entry is a populated form of `get_empty_record()` with the key of each being the read['read_id'].
+ threads = number of threads to use in the C backend.
+ batchsize = number of reads to write at a time. If parsing 1000 records, with batchsize=250 and threads=4, 4 threads will be spawned 4 times to write 250 records to the file before returning
+ aux = an empty aux record returned by `get_empty_record(aux=True)`
+ returns 0 on success and -1 on error/failure

Example:

```python

record, aux = s5.get_empty_record(aux=True)
# populate record, aux
#....
records[record['read_id']] = record
auxs[record['read_id']] = aux
# Write record
ret = s5.write_record_batch(records, threads=2, batchsize=3, aux=auxs)
print("ret: write_record(): {}".format(ret))
```

#### `close()`:

Closes a record open for writing or appending, and writes an End Of File (EOF) flag.

If not explicitly closed, when the `s5` object goes out of context in python, it will also trigger a close to attempt to avoid having a missing EOF.

Please call this when you are finished writing a file.

Example:

```python
s5 = slow5.Open(file,'w')

# do some writing....

# Write's EOF and closes file
s5.close()
```

## Citation

Please cite the following in your publications when using *slow5lib/pyslow5*:

> Gamaarachchi, H., Samarakoon, H., Jenner, S.P. et al. Fast nanopore sequencing data analysis with SLOW5. Nat Biotechnol 40, 1026-1029 (2022). https://doi.org/10.1038/s41587-021-01147-4

```
@article{gamaarachchi2022fast,
  title={Fast nanopore sequencing data analysis with SLOW5},
  author={Gamaarachchi, Hasindu and Samarakoon, Hiruna and Jenner, Sasha P and Ferguson, James M and Amos, Timothy G and Hammond, Jillian M and Saadat, Hassaan and Smith, Martin A and Parameswaran, Sri and Deveson, Ira W},
  journal={Nature biotechnology},
  pages={1--4},
  year={2022},
  publisher={Nature Publishing Group}
}
```
