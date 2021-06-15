
# distutils: language = c++
# cython: language_level=3
import sys
from libc.stdlib cimport malloc, free
from libc.string cimport strdup
cimport pyslow5


cdef class slow5py:
    cdef pyslow5.slow5_file_t *s5p
    cdef pyslow5.slow5_rec_t *rec
    cdef pyslow5.slow5_rec_t *read
    cdef const char* p
    cdef const char* m

    def __cinit__(self, pathname, mode):
        self.s5p = NULL
        self.rec = NULL
        self.read = NULL
        p = str.encode(pathname)
        self.p=p
        m = str.encode(mode)
        self.m=m
        print(self.p, self.m, file=sys.stderr)
        self.s5p = pyslow5.slow5_open(self.p, self.m)
        if not self.s5p:
            raise MemoryError()
        # load or create and load index
        print("Number of read_groups:", self.s5p.header.num_read_groups)
        print("Creating/loading index...", file=sys.stderr)
        ret = slow5_idx_load(self.s5p)
        print("Index returned: ", ret, file=sys.stderr)

    def __dealloc__(self):
        if self.rec is not NULL:
            slow5_rec_free(self.rec)
        if self.read is not NULL:
            slow5_rec_free(self.read)
        if self.s5p is not NULL:
            pyslow5.slow5_close(self.s5p)


    def get_read(self, read_id):
        dic = {}
        ID = str.encode(read_id)
        # rec = NULL
        ret = slow5_get(ID, &self.rec, self.s5p)
        print("get return:", ret, file=sys.stderr)
        # for i in range(20):
        #     print(self.rec.raw_signal[i])
        dic['read_id'] = self.rec.read_id
        dic['read_group'] = self.rec.read_group
        dic['digitisation'] = self.rec.digitisation
        dic['offset'] = self.rec.offset
        dic['range'] = self.rec.range
        dic['sampling_rate'] = self.rec.sampling_rate
        dic['len_raw_signal'] = self.rec.len_raw_signal
        dic['signal'] = []
        for i in range(self.rec.len_raw_signal):
            dic['signal'].append(self.rec.raw_signal[i])
        
        return dic


    def yield_reads(self, pA=False):
        '''
        returns generator for sequential reading of slow5 file
        if pa=True, do pA conversion of signal here.
        '''
        ret = 0
        while ret == 0:
            ret = slow5_get_next(&self.read, self.s5p)
            print("TEST READID", self.read.read_id)
            print("slow5_get_next return:", ret, file=sys.stderr)
            if ret != 0:
                break
            row = {}

            row['read_id'] = self.read.read_id
            row['read_group'] = self.read.read_group
            row['digitisation'] = self.read.digitisation
            row['offset'] = self.read.offset
            row['range'] = self.read.range
            row['sampling_rate'] = self.read.sampling_rate
            row['len_raw_signal'] = self.read.len_raw_signal
            row['signal'] = []
            for i in range(self.read.len_raw_signal):
                row['signal'].append(self.read.raw_signal[i])

            # if pA=True, convert signal to pA
            if pA:
                # call some conversion function
                pass
            yield row
