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
* [slow5_hdr_get](slow5_hdr_get.md)
    Fetch a header data value corresponding to a given attribute ID.

### Low-level API for reading and writing SLOW5 files

* [slow5_open_with](low_level_api/slow5_open_with.md)
    Open a SLOW5 file. User can specify the SLOW5 format.
