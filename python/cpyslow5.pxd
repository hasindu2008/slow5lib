cdef extern from "pyslow5.h":
    ctypedef struct slow5_file_t:
        pass

    # Open a slow5 file
    slow5_file_t slow5_open(const char *pathname, const char *mode)
    int slow5_close(slow5_file_t* s5p)
    int slow5_idx_create(slow5_file_t* s5p)
