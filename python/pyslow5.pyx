
# distutils: language = c++
# cython: language_level=3
import sys
from libc.stdlib cimport malloc, free
from libc.string cimport strdup
cimport pyslow5


cdef class slow5py:
    cdef pyslow5.slow5_file_t *s5p
    cdef pyslow5.slow5_rec_t *rec
    cdef const char* p
    cdef const char* m

    def __cinit__(self, pathname, mode):
        # self.s5p = NULL
        p = str.encode(pathname)
        self.p=p
        m = str.encode(mode)
        self.m=m
        print(self.p, self.m, file=sys.stderr)
        self.s5p = pyslow5.slow5_open(self.p, self.m)
        print("file pointer", self.s5p.fp)
        if not self.s5p:
            raise MemoryError()

    def __dealloc__(self):
        if self.s5p is not NULL:
            pyslow5.slow5_close(self.s5p)


    def get_read(self, read_id):
        print("Creating index...", file=sys.stderr)
        ret = slow5_idx_load(self.s5p)
        print("Index returned: ", ret, file=sys.stderr)
        ID = str.encode(read_id)
        # rec = NULL
        ret = slow5_get(ID, &self.rec, self.s5p)
        print("get return:", ret, file=sys.stderr)
        for i in range(20):
            print(self.rec.raw_signal[i])
