# slow5lib

**Note: Low-level API documentation is currently sparse. If something is not clear do not hesitate to open an [issue](https://github.com/hasindu2008/slow5lib/issues).**

## Low-level API for reading and writing SLOW5 files

Low-level API allows much more efficient access to BLOW5 files compared to the high-level API.  Following documented functions are stable.

### Common

* [slow5_set_log_level](low_level_api/slow5_set_log_level.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;sets the level log message verbosity
* [slow5_set_exit_condition](low_level_api/slow5_set_exit_condition.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;sets if slow5lib should exit the programme on error


### Reading and access

* [slow5_get_rids](low_level_api/slow5_get_rids.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;gets the pointer to the list of read IDs associated with a SLOW5 file
* [slow5_get_hdr_keys](low_level_api/slow5_get_hdr_keys.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;gets the list of data header attribute keys
* [slow5_get_aux_names](low_level_api/slow5_get_aux_names.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;gets the pointer to the list of auxiliary field names
* [slow5_get_next_bytes](low_level_api/slow5_get_next_bytes.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;fetches the raw record (without decompressing or parsing) at the current file pointer of a SLOW5 file
* [slow5_decode](low_level_api/slow5_decode.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;decodes a slow5 record
* [slow5_aux_get_enum](low_level_api/slow5_aux_get_enum.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;fetches an auxiliary field of type enum from a SLOW5 record
* [slow5_get_aux_enum_labels](low_level_api/slow5_get_aux_enum_labels.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;gets the list of labels for an enum data type

### Writing and editing

* [slow5_aux_add_enum](low_level_api/slow5_aux_add_enum.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;adds an auxiliary field of type enum to a SLOW5 header
* [slow5_encode](low_level_api/slow5_encode.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;encodes a SLOW5 record
* [slow5_write_bytes](low_level_api/slow5_write_bytes.md)<br/>
  &nbsp;&nbsp;&nbsp;&nbsp;writes an encoded SLOW5 record to a SLOW5 file


**If you want any other functionality than what is described above, please open a GitHub issue so such functionality exposed, rather than trying to use internal functions by browsing the code. Such undocumented functions are subject to change.**
