
# distutils: language = c++
# cython: language_level=3
import sys
import logging
from libc.stdlib cimport malloc, free
from libc.string cimport strdup
cimport pyslow5

#
# Class slow5py is for read-only of slow5 files.
#


cdef class slow5py:
    cdef pyslow5.slow5_file_t *s5p
    cdef pyslow5.slow5_rec_t *rec
    cdef pyslow5.slow5_rec_t *read
    cdef pyslow5.uint64_t head_len
    cdef const char* p
    cdef const char* m
    cdef int V
    cdef object logger

    def __cinit__(self, pathname, mode, DEBUG=0):

        # Set NULL for cleanup check end MemoryError check
        self.s5p = NULL
        self.rec = NULL
        self.read = NULL
        self.head_len = 0
        self.V = DEBUG
        self.logger = logging.getLogger(__name__)
        if self.V ==1:
            lev = logging.DEBUG
        else:
            lev = logging.WARNING

        logging.basicConfig(format='%(asctime)s - [%(levelname)s]: %(message)s',
                            datefmt='%d-%b-%y %H:%M:%S', level=lev)

        p = str.encode(pathname)
        self.p=p
        m = str.encode(mode)
        self.m=m
        print(self.p, self.m, file=sys.stderr)
        self.s5p = pyslow5.slow5_open(self.p, self.m)
        if self.s5p is NULL:
            raise MemoryError()
        # load or create and load index
        self.logger.debug(f"Number of read_groups: {self.s5p.header.num_read_groups}")
        self.logger.debug(f"Creating/loading index...")
        ret = slow5_idx_load(self.s5p)
        if ret != 0:
            self.logger.debug(f"slow5_idx_load return not 0: {ret}")

    def __dealloc__(self):
        if self.rec is not NULL:
            slow5_rec_free(self.rec)
        if self.read is not NULL:
            slow5_rec_free(self.read)
        if self.s5p is not NULL:
            pyslow5.slow5_close(self.s5p)


    def _convert_to_pA(self, d):
        '''
        convert raw signal data to pA using digitisation, offset, and range
        float raw_unit = range / digitisation;
        for (int32_t j = 0; j < nsample; j++) {
            rawptr[j] = (rawptr[j] + offset) * raw_unit;
        }
        '''
        digitisation = d['digitisation']
        range = d['range']
        offset = d['offset']
        raw_unit = range / digitisation
        new_raw = []
        for i in d['signal']:
            j = (i + offset) * raw_unit
            new_raw.append(float("{0:.2f}".format(round(j,2))))
        return new_raw


    def _get_read(self, read_id, pA):
        dic = {}
        ID = str.encode(read_id)
        # rec = NULL
        ret = slow5_get(ID, &self.rec, self.s5p)
        if ret != 0:
            self.logger.debug(f"get_read return not 0: {ret}")
            return None
        # for i in range(20):
        #     print(self.rec.raw_signal[i])
        if type(self.rec.read_id) is bytes:
            dic['read_id'] = self.rec.read_id.decode()
        else:
            dic['read_id'] = self.rec.read_id
        dic['read_group'] = self.rec.read_group
        dic['digitisation'] = self.rec.digitisation
        dic['offset'] = self.rec.offset
        dic['range'] = self.rec.range
        dic['sampling_rate'] = self.rec.sampling_rate
        dic['len_raw_signal'] = self.rec.len_raw_signal
        dic['signal'] = [self.rec.raw_signal[i] for i in range(self.rec.len_raw_signal)]
        # This could be handled by numpy.fromiter() or similar
        # Probably MUCH faster
        # https://stackoverflow.com/questions/7543675/how-to-convert-pointer-to-c-array-to-python-array
        # for i in range(self.rec.len_raw_signal):
        #     dic['signal'].append(self.rec.raw_signal[i])
        # if pA=True, convert signal to pA
        if pA:
            dic['signal'] = self._convert_to_pA(dic)

        return dic

    def get_read(self, read_id, pA=False):
        return self._get_read(read_id, pA)


    def seq_reads(self, pA=False):
        '''
        returns generator for sequential reading of slow5 file
        if pa=True, do pA conversion of signal here.
        TODO: Add aux=None, take single, list, "all" for aux fields
        '''
        ret = 0
        while ret == 0:
            ret = slow5_get_next(&self.read, self.s5p)
            self.logger.debug(f"slow5_get_next return: {ret}")
            # need a ret code for EOF to handle better
            # -2 is the current ret when EOF is hit, but it's not specific
            if ret != 0:
                self.logger.debug(f"slow5_get_next reached end of record (-2): {ret}")
                break

            row = {}
            if type(self.read.read_id) is bytes:
                row['read_id'] = self.read.read_id.decode()
            else:
                row['read_id'] = self.read.read_id
            # row['read_id'] = self.read.read_id.decode()
            row['read_group'] = self.read.read_group
            row['digitisation'] = self.read.digitisation
            row['offset'] = self.read.offset
            row['range'] = self.read.range
            row['sampling_rate'] = self.read.sampling_rate
            row['len_raw_signal'] = self.read.len_raw_signal
            row['signal'] = [self.read.raw_signal[i] for i in range(self.read.len_raw_signal)]
            # for i in range(self.read.len_raw_signal):
            #     row['signal'].append(self.read.raw_signal[i])

            # if pA=True, convert signal to pA
            if pA:
                row['signal'] = self._convert_to_pA(row)

            yield row


    def get_read_list(self, read_list, pA=False):
        '''
        returns generator for sequential reading of slow5 file
        if pa=True, do pA conversion of signal here.
        '''
        for r in read_list:
            yield self._get_read(r, pA)


    def get_header_names(self):
        '''
        TODO
        get all header names and return list
        '''
        headers = []
        # ret = slow5_header_names(self.s5p.header)
        ret = slow5_get_hdr_keys(self.s5p.header, &self.head_len)

        self.logger.debug(f"slow5_get_hdr_keys head_len: {self.head_len}")
        if ret == NULL:
            self.logger.debug(f"slow5_get_hdr_keys ret is NULL")
            return headers

        headers = [ret[i].decode() for i in range(self.head_len)]
        return headers

    def get_header_value(self, attr, read_group=0):
        '''
        get header attribute value
        '''
        a = str.encode(attr)
        ret = slow5_hdr_get(a, read_group, self.s5p.header).decode()
        return ret
