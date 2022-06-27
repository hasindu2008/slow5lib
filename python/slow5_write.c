#include <stdio.h>
#include <slow5/slow5.h>

extern enum slow5_log_level_opt  slow5_log_level;
extern enum slow5_exit_condition_opt  slow5_exit_condition;

slow5_file_t *slow5_open_write(const char *filename, const char *mode){
    return slow5_open(filename,"w");
}

slow5_file_t *slow5_open_write_append(const char *filename, const char *mode){
    return slow5_open(filename,"a");
}


int slow5_close_write(slow5_file_t *sf){
    return slow5_close(sf);
}

int slow5_header_write(slow5_file_t *sf){
    return slow5_hdr_write(sf);
}

int slow5_rec_write(slow5_file_t *sf, slow5_rec_t *rec){
    return slow5_write(rec,sf);
}

int slow5_aux_meta_add_wrapper(slow5_hdr_t *header, const char *field, enum slow5_aux_type type){
    return slow5_aux_add(field, type, header);
}

int slow5_rec_set_wrapper(struct slow5_rec *read, slow5_hdr_t *header, const char *field, const void *data){
    return slow5_aux_set(read, field, data, header);
}

int slow5_rec_set_string_wrapper(struct slow5_rec *read, slow5_hdr_t *header, const char *field, const char *data) {
    return slow5_aux_set_string(read, field, data, header);
}

