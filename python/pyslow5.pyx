
# distutils: language = c++
# cython: language_level=3

from libc.stdlib cimport malloc, free
from libc.string cimport strdup
cimport pyslow5


cdef class slow5py:
    cdef pyslow5.slow5_file_t* _s5p

    def __dealloc__(self):
        if self._s5p is not NULL:
            pyslow5.slow5_close(self._s5p)

    def __init__(self, pathname, mode):
        self.p = pathname
        self.m = mode
        print(self.p, self.m, file=sys.stderr)
        self._s5p = pyslow5.slow5_open(<const char *>self.p, <const char *>self.m)
        if self._s5p is NULL:
            raise MemoryError()

    def index_create(self):
        ''' Create index '''
        print("Creating index...", file=sys.stderr)
        ret = slow5_idx_create(self._s5p)
        if ret != 0:
            print("Error creating index, exit code: {}".format(ret), file=sys.stderr)
