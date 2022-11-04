# slow5lib

**Note: This documentation is far from being complete and perfect. So if you need a new functionality, want to clarify something, find something is not right, facing a weird bug when using this API etc. do not hesitate to open an [issue](https://github.com/hasindu2008/slow5lib/issues). Nowadays, bioinformatics tools written in C/C++ are getting fewer and fewer, so I would be glad to help.**

## NAME

slow5lib - slow5 Overview

## DESCRIPTION

slow5lib is a library for reading and writing SLOW5 files. Compiling slow5lib requires a C compiler that conforms to at least c99 standard with X/Open 7, incorporating POSIX 2008 extension support.

### Data Structures

The *slow5_file_t* structure stores the file pointer, parsed SLOW5 header and other metadata of an opened SLOW5 file.
The user can directly access the struct member *slow5_hdr_t *header* which contains the pointer to the parsed SLOW5 header.
Other struct members are private and not supposed to be directly accessed.

The *slow5_hdr_t* structure stores a parsed SLOW5 header. This structure has the following form:

```
typedef struct {

    /* private struct members that are not supposed to be directly accessed are not shown.
       the order of the memebers in this struct can subject to change.
    */

    struct slow5_version version;       // SLOW5 file version
    uint32_t num_read_groups;           // Number of read groups

} slow5_hdr_t;
```

The slow5_version structure contains the major, minor and patch version as follows:

```
struct slow5_version {
    uint8_t major;  // major version
    uint8_t minor;  // minor version
    uint8_t patch;  // patch version
};
```

The *slow5_rec_t* structure stores a parsed slow5 record. This structure has the following form:

```
typedef struct {
    slow5_rid_len_t read_id_len;        // length of the read ID string (does not include null character)
    char* read_id;                      // the read ID
    uint32_t read_group;                // the read group
    double digitisation;                // the number of quantisation levels - required to convert the signal to pico ampere
    double offset;                      // offset value - required to convert the signal to pico ampere
    double range;                       // range value - required to convert to pico ampere
    double sampling_rate;               // the sampling rate at which the signal was acquired
    uint64_t len_raw_signal;            // length of the raw signal array
    int16_t* raw_signal;                // the actual raw signal array

    /* Other private members for storing auxilliary field which are not to be directly accessed*/

} slow5_rec_t;
```


### High-level API

High-level API consists of following functions:

#### Common

* [slow5_open](slow5_open.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;opens a SLOW5 file
* [slow5_close](slow5_close.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;closes a  SLOW5 file
* [slow5_idx_load](slow5_idx_load.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;loads the index file for a SLOW5 file
* [slow5_idx_unload](slow5_idx_unload.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;unloads a SLOW5 index from the memory
* [slow5_idx_create](slow5_idx_create.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;creates an index file for a SLOW5 file
* [slow5_rec_free](slow5_rec_free.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;frees up a SLOW5 record from memory

### Reading
* [slow5_get](slow5_get.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;fetches a record corresponding to a given read ID
* [slow5_get_next](slow5_get_next.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;fetches the record at the current file pointer of a SLOW5 file
* [slow5_hdr_get](slow5_hdr_get.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;fetches a header data attribute from a SLOW5 header
* [slow5_aux_get\_*\<primitive_datatype\>*](slow5_aux_get.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;fetches an auxiliary field (a primitive datatype) from a SLOW5 record. Following functions are available:
    - [slow5_aux_get_int8](slow5_aux_get.md)
    - [slow5_aux_get_int16](slow5_aux_get.md)
    - [slow5_aux_get_int32](slow5_aux_get.md)
    - [slow5_aux_get_int64](slow5_aux_get.md)
    - [slow5_aux_get_uint8](slow5_aux_get.md)
    - [slow5_aux_get_uint16](slow5_aux_get.md)
    - [slow5_aux_get_uint32](slow5_aux_get.md)
    - [slow5_aux_get_uint64](slow5_aux_get.md)
    - [slow5_aux_get_float](slow5_aux_get.md)
    - [slow5_aux_get_double](slow5_aux_get.md)
    - [slow5_aux_get_char](slow5_aux_get.md)
*   [slow5_aux_get\_*\<array_datatype\>*](slow5_aux_get_array.md)<br/>
    &nbsp;&nbsp;&nbsp;&nbsp;fetches an auxiliary field (an array datatype) of a SLOW5 record. Following functions are available:
    * [slow5_aux_get_int8_array](slow5_aux_get_array.md)
    * [slow5_aux_get_int16_array](slow5_aux_get_array.md)
    * [slow5_aux_get_int32_array](slow5_aux_get_array.md)
    * [slow5_aux_get_int64_array](slow5_aux_get_array.md)
    * [slow5_aux_get_uint8_array](slow5_aux_get_array.md)
    * [slow5_aux_get_uint16_array](slow5_aux_get_array.md)
    * [slow5_aux_get_uint32_array](slow5_aux_get_array.md)
    * [slow5_aux_get_uint64_array](slow5_aux_get_array.md)
    * [slow5_aux_get_float_array](slow5_aux_get_array.md)
    * [slow5_aux_get_double_array](slow5_aux_get_array.md)
    * [slow5_aux_get_string](slow5_aux_get_array.md)

### Writing

* [slow5_hdr_add](slow5_hdr_add.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;adds a new header data attribute to a SLOW5 header
* [slow5_hdr_set](slow5_hdr_set.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;sets a header data attribute for a particular read_group
* [slow5_aux_add](slow5_aux_add.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;adds an auxiliary field to a SLOW5 header
* [slow5_hdr_write](slow5_hdr_write.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;writes the associated SLOW5 header to a SLOW5 file
* [slow5_rec_init](slow5_rec_init.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;initialises an empty SLOW5 record
* [slow5_aux_set](slow5_aux_set.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;sets an auxiliary field (a primitive datatype) of a SLOW5 record
* [slow5_aux_set_string](slow5_aux_set_string.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;sets an auxiliary field (string datatype) of a SLOW5 record
* [slow5_write](slow5_write.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;writes a SLOW5 record to a SLOW5 file
* [slow5_set_press](slow5_set_press.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;sets the compression method for a BLOW5 file opened for writing



### Low-level API for reading and writing SLOW5 files

Low-level API allows much more efficient access to BLOW5 files compared to the high-level API.  Low-level API is documented [here](slow5_low_level_api.md).


## CITATION

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


<!--
### Low-level API for reading and writing SLOW5 files
* [slow5_open_with](low_level_api/slow5_open_with.md)
    Open a SLOW5 file. User can specify the SLOW5 format.

adding read groups
setting different compression

-->
