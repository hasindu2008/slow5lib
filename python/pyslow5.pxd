#cython: language_level=3
from libc.stdio cimport *
from libc.stdint cimport *
from libc.stdlib cimport *

cdef extern from "pyslow5.h":

    cdef enum slow5_aux_type:
        SLOW5_INT8_T = 0,
        SLOW5_INT16_T,
        SLOW5_INT32_T,
        SLOW5_INT64_T,
        SLOW5_UINT8_T,
        SLOW5_UINT16_T,
        SLOW5_UINT32_T,
        SLOW5_UINT64_T,
        SLOW5_FLOAT,
        SLOW5_DOUBLE,
        SLOW5_CHAR,
        SLOW5_ENUM,
        SLOW5_INT8_T_ARRAY,
        SLOW5_INT16_T_ARRAY,
        SLOW5_INT32_T_ARRAY,
        SLOW5_INT64_T_ARRAY,
        SLOW5_UINT8_T_ARRAY,
        SLOW5_UINT16_T_ARRAY,
        SLOW5_UINT32_T_ARRAY,
        SLOW5_UINT64_T_ARRAY,
        SLOW5_FLOAT_ARRAY,
        SLOW5_DOUBLE_ARRAY,
        SLOW5_STRING,
        SLOW5_ENUM_ARRAY

    ctypedef struct slow5_aux_meta_t:
        pass

    ctypedef struct slow5_hdr_data_t:
        pass

    ctypedef struct slow5_version:
        uint8_t major
        uint8_t minor
        uint8_t patch

    ctypedef struct slow5_press_t:
        pass
    ctypedef struct slow5_hdr_t:
        slow5_version version;
        uint32_t num_read_groups;
        slow5_hdr_data_t data;
        slow5_aux_meta_t *aux_meta;

    ctypedef struct slow5_idx_t:
        pass
    ctypedef enum slow5_fmt:
        pass
    ctypedef struct slow5_file_meta_t:
        pass

    ctypedef struct slow5_file_t:
        FILE *fp
        slow5_fmt format
        slow5_press_t *compress
        slow5_hdr_t *header
        slow5_idx_t *index
        slow5_file_meta_t meta

    ctypedef struct slow5_rec_t:
        uint16_t read_id_len
        char* read_id
        uint32_t read_group
        double digitisation
        double offset
        double range
        double sampling_rate
        uint64_t len_raw_signal
        int16_t* raw_signal
        pass


    # Open a slow5 file
    slow5_file_t *slow5_open(const char *pathname, const char *mode);
    const char **slow5_get_hdr_keys(const slow5_hdr_t *header, uint64_t *len);
    char *slow5_hdr_get(const char *attr, uint32_t read_group, const slow5_hdr_t *header);
    void slow5_idx_unload(slow5_file_t *s5p);
    int slow5_close(slow5_file_t *s5p);
    int slow5_idx_load(slow5_file_t *s5p);
    int slow5_get(const char *read_id, slow5_rec_t **read, slow5_file_t *s5p);
    int slow5_get_next(slow5_rec_t **read, slow5_file_t *s5p);
    char **slow5_get_aux_names(const slow5_hdr_t *header, uint64_t *len);
    slow5_aux_type *slow5_get_aux_types(const slow5_hdr_t *header, uint64_t *len);
    void slow5_rec_free(slow5_rec_t *read);
    char **slow5_get_rids(const slow5_file_t *s5p, uint64_t *len);


    # get aux fields
    int8_t slow5_aux_get_int8(const slow5_rec_t *read, const char *attr, int *err);
    int16_t slow5_aux_get_int16(const slow5_rec_t *read, const char *attr, int *err);
    int32_t slow5_aux_get_int32(const slow5_rec_t *read, const char *attr, int *err);
    int64_t slow5_aux_get_int64(const slow5_rec_t *read, const char *attr, int *err);
    uint8_t slow5_aux_get_uint8(const slow5_rec_t *read, const char *attr, int *err);
    uint16_t slow5_aux_get_uint16(const slow5_rec_t *read, const char *attr, int *err);
    uint32_t slow5_aux_get_uint32(const slow5_rec_t *read, const char *attr, int *err);
    uint64_t slow5_aux_get_uint64(const slow5_rec_t *read, const char *attr, int *err);
    float slow5_aux_get_float(const slow5_rec_t *read, const char *attr, int *err);
    double slow5_aux_get_double(const slow5_rec_t *read, const char *attr, int *err);
    char slow5_aux_get_char(const slow5_rec_t *read, const char *attr, int *err);
    uint8_t slow5_aux_get_enum(const slow5_rec_t *read, const char *field, int *err);
    
    # get aux arrays
    int8_t *slow5_aux_get_int8_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err);
    int16_t *slow5_aux_get_int16_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err);
    int32_t *slow5_aux_get_int32_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err);
    int64_t *slow5_aux_get_int64_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err);
    uint8_t *slow5_aux_get_uint8_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err);
    uint16_t *slow5_aux_get_uint16_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err);
    uint32_t *slow5_aux_get_uint32_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err);
    uint64_t *slow5_aux_get_uint64_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err);
    float *slow5_aux_get_float_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err);
    double *slow5_aux_get_double_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err);
    char *slow5_aux_get_string(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err);
    uint8_t *slow5_aux_get_enum_array(const slow5_rec_t *read, const char *field, uint64_t *len, int *err);

    # get aux enum labels
    char **slow5_get_aux_enum_labels(const slow5_hdr_t *header, const char *field, uint8_t *n);


    # Write slow5 file

    # from slow5.h
    int slow5_set_press(slow5_file_t *s5p, int rec_press, int sig_press);
    int slow5_hdr_add_attr(const char *attr, slow5_hdr_t *header);
    int slow5_hdr_set(const char *attr, const char *value, uint32_t read_group, slow5_hdr_t *header);
    int64_t slow5_hdr_add_rg(slow5_hdr_t *header);
    slow5_rec_t *slow5_rec_init();

    # Write slow5 file
    int slow5_hdr_write(slow5_file_t *sf);
    int slow5_write(slow5_rec_t *rec, slow5_file_t *sf);
    int slow5_aux_add(const char *attr, slow5_aux_type type, slow5_hdr_t *header);
    int slow5_aux_add_enum(const char *field, const char **enum_labels, uint8_t num_labels, slow5_hdr_t *header)
    int slow5_aux_set(slow5_rec_t *read, const char *attr, const void *data, slow5_hdr_t *header);
    int slow5_aux_set_string(slow5_rec_t *read, const char *attr, const char *data, slow5_hdr_t *header);

    int slow5_get_batch_lazy(slow5_rec_t ***read, slow5_file_t *s5p, char **rid, int num_rid, int num_threads);
    int slow5_get_next_batch_lazy(slow5_rec_t ***read, slow5_file_t *s5p, int batch_size, int num_threads);
    int slow5_write_batch_lazy(slow5_rec_t **read, slow5_file_t *s5p, int batch_size, int num_threads);
    void slow5_free_batch_lazy(slow5_rec_t ***read, int num_rec);
