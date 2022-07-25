# slow5lib

## Low-level API for reading and writing SLOW5 files

Low-level API allows much more efficient access to BLOW5 files compared to the high-level API.  However, they are not yet finalised and this documentation is for internal use.
If anyone is interested in using functions here, please open a GitHub issue. Unless, finalised and documented, these function prototypes are subject to change.

### Common

* [slow5_set_log_level](low_level_api/slow5_set_log_level.md)<br/>
* [slow5_set_exit_condition](low_level_api/slow5_set_exit_condition.md)<br/>
* [slow5_open_with](low_level_api/slow5_open_with.md)<br/>
    Open a SLOW5 file. User can specify the SLOW5 format.

### Reading and access

* [slow5_get_rids](low_level_api/slow5_get_rids.md)<br/>
* [slow5_get_hdr_keys](low_level_api/slow5_get_hdr_keys.md)<br/>
* [slow5_get_aux_names](low_level_api/slow5_get_aux_names.md)<br/>
* [slow5_get_aux_types](low_level_api/slow5_get_aux_types.md)<br/>

* [slow5_get_next_bytes](low_level_api/slow5_get_next_bytes.md)<br/>
* [slow_decode](low_level_api/slow_decode.md)<br/>
* [slow5_get_next_mem](low_level_api/slow5_get_next_mem.md)<br/>
* [slow5_rec_depress_parse](low_level_api/slow5_rec_depress_parse.md)<br/>

* [slow5_get_aux_enum_labels](low_level_api/slow5_get_aux_enum_labels.md)<br/>
* [slow5_aux_get_enum](low_level_api/slow5_aux_get_enum.md)<br/>
* [slow5_aux_get_enum_array](low_level_api/slow5_aux_get_enum_array.md)<br/>

### Writing and editing

* [slow5_encode](low_level_api/slow5_rec_to_mem.md)<br/>
	Encodes a SLOW5 record to linear memory.
* [slow5_write_bytes](low_level_api/slow5_write_bytes.md)
* [slow5_rec_to_mem]
	Encodes a SLOW5 record to linear memory.
* [slow5_rec_fwrite]
	Print a SLOW5 record in the specified format to a file pointer.
* [slow5_rec_print]
	Print a SLOW5 record in the specified format to standard output.
* [slow5_hdr_add_attr]
	Adds a new header data attribute.
* [slow5_hdr_add_rg]
	Adds a new header read group.
* [slow5_hdr_to_mem]
	Encodes a header in to linear memory.
* slow5_hdr_fwrite
* slow5_hdr_print
* slow5_eof_fwrite
* slow5_eof_print
* slow5_press_init
* slow5_press_free
* slow5_aux_meta_add_enum
* slow5_aux_meta_add
* slow5_rec_set
* slow5_rec_set_string
* slow5_aux_meta_init_empty
* slow5_hdr_set
* slow5_hdr_add_rg_data

<!--
int slow5_hdr_fwrite(FILE *fp, struct slow5_hdr *header, enum slow5_fmt format, slow5_press_method_t comp)
int slow5_rec_fwrite(FILE *fp, struct slow5_rec *read, struct slow5_aux_meta *aux_meta, enum slow5_fmt format, struct slow5_press *compress)
int slow5_aux_meta_add_enum(struct slow5_aux_meta *aux_meta, const char *attr, enum slow5_aux_type type, const char **enum_labels, uint8_t enum_num_labels)
int slow5_aux_meta_add(struct slow5_aux_meta *aux_meta, const char *attr, enum slow5_aux_type type)
int slow5_rec_set(struct slow5_rec *read, struct slow5_aux_meta *aux_meta, const char *attr, const void *data)
static inline int slow5_rec_set_string(struct slow5_rec *read, struct slow5_aux_meta *aux_meta, const char *attr, const char *data)
struct slow5_aux_meta *slow5_aux_meta_init_empty(void)
int slow5_hdr_add_attr(const char *attr, struct slow5_hdr *header)
int slow5_hdr_set(const char *attr, const char *value, uint32_t read_group, struct slow5_hdr *header)
struct slow5_aux_meta *slow5_aux_meta_init_empty(void)
int64_t slow5_hdr_add_rg_data(struct slow5_hdr *header, khash_t(slow5_s2s) *new_data)
struct slow5_press *slow5_press_init(slow5_press_method_t method)
void *slow5_rec_to_mem(struct slow5_rec *read, struct slow5_aux_meta *aux_meta, enum slow5_fmt format, struct slow5_press *compress, size_t *n)
void slow5_press_free(struct slow5_press *comp)

-->