#ifndef PYSLOW5_WRITE_H
#define PYSLOW5_WRITE_H

//these functionalities will be moved to the slow5lib API in a while, but names will change
slow5_file_t *slow5_open_write(const char *filename, const char *mode);
slow5_file_t *slow5_open_write_append(const char *filename, const char *mode);
int slow5_close_write(slow5_file_t *sf);
int slow5_header_write(slow5_file_t *sf);
int slow5_rec_write(slow5_file_t *sf, slow5_rec_t *rec);
int slow5_aux_meta_add_wrapper(slow5_hdr_t *header, const char *attr, enum slow5_aux_type type);
int slow5_rec_set_wrapper(struct slow5_rec *read, slow5_hdr_t *header, const char *attr, const void *data);
int slow5_rec_set_string_wrapper(struct slow5_rec *read, slow5_hdr_t *header, const char *attr, const char *data);

#endif
