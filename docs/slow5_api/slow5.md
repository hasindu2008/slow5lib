# slow5lib

## NAME

slow5lib - slow5 Overview

## DESCRIPTION

slow5lib is a library for reading and writing SLOW5 files.

slow5lib interface consists of following functions:

### High-level API for reading SLOW5 files

* [slow5_open](slow5_open.md)
    Open a SLOW5 file.
* [slow5_close](slow5_close.md)
    Close a  SLOW5 file.
* [slow5_get](slow5_open.md)
    Fetch a record corresponding to a given read ID.
* [slow5_get_next](slow5_get_next.md)
    Fetch the next record.
* [slow5_idx_create](slow5_idx_create.md)
    Create the index file.
* [slow5_idx_load](slow5_idx_load.md)
    Load the index file into memory.
* [slow5_idx_unload](slow5_idx_unload.md)
    Unoad the index file from memory.
* [slow5_rec_free](slow5_rec_free.md)
    Free a slow5 record.
* [slow5_hdr_get](slow5_hdr_get.md)
    Fetch a header data value corresponding to a given attribute ID.
*  slow5_aux_get_<datatype>
    Get an auxiliary field (a primitive datatype) of a SLOW5 record. Following functions are available:
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

*   slow5_aux_get_<datatype-array>
    Get an auxiliary field (an array datatype) of a SLOW5 record. Following functions are available:
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

<!--
### Low-level API for reading and writing SLOW5 files
* [slow5_open_with](low_level_api/slow5_open_with.md)
    Open a SLOW5 file. User can specify the SLOW5 format.
-->

### Note

Compiling slow5lib requires a C compiler that conforms to at least c99 standard with X/Open 7, incorporating POSIX 2008 extension support.
