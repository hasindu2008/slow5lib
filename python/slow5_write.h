#ifndef PYSLOW5_WRITE_H
#define PYSLOW5_WRITE_H

slow5_file_t *slow5_open_write(char *filename, char *mode);
int slow5_close_write(slow5_file_t *sf);
int slow5_header_write(slow5_file_t *sf);
int slow5_rec_write(slow5_file_t *sf, slow5_rec_t *rec);

#endif