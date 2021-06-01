from libc.stdint cimport *


cdef extern from "pyslow5.h":

    ctypedef signed char FILE

    ctypedef struct press_t:
        pass
    ctypedef struct slow5_hdr_t:
        pass
    ctypedef struct slow5_idx_t:
        pass
    cdef enum slow5_fmt:
        pass
    ctypedef struct slow5_file_meta_t:
        pass

    ctypedef struct slow5_file_t:
        FILE *fp
        slow5_fmt format
        press_t *compress
        slow5_hdr_t *header
        slow5_idx_t *index
        slow5_file_meta_t meta


    ctypedef struct slow5_rec_t:
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
    slow5_file_t *slow5_open(const char *pathname, const char *mode)
    int slow5_close(slow5_file_t *s5p)
    int slow5_idx_load(slow5_file_t *s5p)
    int slow5_get(const char *read_id, slow5_rec_t **read, slow5_file_t *s5p)
