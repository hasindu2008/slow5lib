# slow5lib

## Low-level API for reading and writing SLOW5 files

Low-level API allows much more efficient access to BLOW5 files compared to the high-level API.  However, they are not yet finalised and this documentation is for internal use.
If anyone is interested in using functions here, please open a GitHub issue. Unless, finalised and documented, these function prototypes are subject to change.

### Common

* [slow5_set_log_level](low_level_api/slow5_set_log_level.md)<br/>
* [slow5_set_exit_condition](low_level_api/slow5_set_exit_condition.md)<br/>


### Reading and access

* [slow5_get_rids](low_level_api/slow5_get_rids.md)<br/>
* [slow5_get_hdr_keys](low_level_api/slow5_get_hdr_keys.md)<br/>
* [slow5_get_aux_names](low_level_api/slow5_get_aux_names.md)<br/>
* [slow5_get_aux_types](low_level_api/slow5_get_aux_types.md)<br/>

* [slow5_get_next_bytes](low_level_api/slow5_get_next_bytes.md)<br/>
* [slow_decode](low_level_api/slow_decode.md)<br/>


### Writing and editing

* [slow5_encode](low_level_api/encode.md)<br/>
	Encodes a SLOW5 record to linear memory.
* [slow5_write_bytes](low_level_api/slow5_write_bytes.md)
