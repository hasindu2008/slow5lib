
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


cdef class Open:
    cdef pyslow5.slow5_file_t *s5p
    cdef pyslow5.slow5_rec_t *rec
    cdef pyslow5.slow5_rec_t *read
    cdef pyslow5.uint64_t head_len
    cdef pyslow5.uint64_t aux_len
    cdef pyslow5.slow5_aux_type *s5_aux_type
    cdef int aux_get_err
    cdef pyslow5.uint64_t aux_get_len
    cdef const char* p
    cdef const char* m
    cdef int V
    cdef object logger
    cdef list aux_names
    cdef list aux_types
    cdef pyslow5.int8_t e0
    cdef pyslow5.int16_t e1
    cdef pyslow5.int32_t e2
    cdef pyslow5.int64_t e3
    cdef pyslow5.uint8_t e4
    cdef pyslow5.uint16_t e5
    cdef pyslow5.uint32_t e6
    cdef pyslow5.uint64_t e7
    cdef pyslow5.float e8
    cdef double e9
    cdef char e10
    cdef pyslow5.int8_t *e11
    cdef pyslow5.int16_t *e12
    cdef pyslow5.int32_t *e13
    cdef pyslow5.int64_t *e14
    cdef pyslow5.uint8_t *e15
    cdef pyslow5.uint16_t *e16
    cdef pyslow5.uint32_t *e17
    cdef pyslow5.uint64_t *e18
    cdef float *e19
    cdef double *e20
    cdef char *e21 # meant to be string, not sure about this


    def __cinit__(self, pathname, mode, DEBUG=0):

        # Set NULL for cleanup check end MemoryError check
        self.s5p = NULL
        self.rec = NULL
        self.read = NULL
        self.s5_aux_type = NULL
        self.aux_get_err = 1
        self.aux_get_len = 0
        self.head_len = 0
        self.aux_len = 0
        self.e0 = -1
        self.e1 = -1
        self.e2 = -1
        self.e3 = -1
        self.e4 = -1
        self.e5 = -1
        self.e6 = -1
        self.e7 = -1
        self.e8 = -1.0
        self.e9 = -1.0
        self.e10 = 0
        self.e11 = NULL
        self.e12 = NULL
        self.e13 = NULL
        self.e14 = NULL
        self.e15 = NULL
        self.e16 = NULL
        self.e17 = NULL
        self.e18 = NULL
        self.e19 = NULL
        self.e20 = NULL
        self.e21 = NULL
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
        self.logger.debug("Number of read_groups: {}".format(self.s5p.header.num_read_groups))
        self.logger.debug("Creating/loading index...")
        ret = slow5_idx_load(self.s5p)
        if ret != 0:
            self.logger.warning("slow5_idx_load return not 0: {}".format(ret))


    def __init__(self, pathname, mode, DEBUG=0):
        self.aux_names = []
        self.aux_types = []

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


    def _get_read(self, read_id, pA, aux):
        dic = {}
        aux_dic = {}
        ID = str.encode(read_id)
        # rec = NULL
        ret = slow5_get(ID, &self.rec, self.s5p)
        if ret != 0:
            self.logger.debug("get_read return not 0: {}".format(ret))
            return None

        if aux is not None:
            if not self.aux_names or not self.aux_types:
                self.aux_names = self.get_aux_names()
                self.aux_types = self.get_aux_types()
            if type(aux) is str:
                if aux == "all":
                    aux_dic = self._get_read_aux(self.aux_names, self.aux_types)
                else:
                    found_single_aux = False
                    for n, t in zip(self.aux_names, self.aux_types):
                        if n == aux:
                            found_single_aux = True
                            aux_dic = self._get_read_aux([n], [t])
                            break
                    if not found_single_aux:
                        self.logger.warning("get_read unknown aux name: {}".format(aux))
                        aux_dic.update({aux: None})
            elif type(aux) is list:
                n_list = []
                t_list = []
                for n, t in zip(self.aux_names, self.aux_types):
                    if n in aux:
                        n_list.append(n)
                        t_list.append(t)

                aux_dic = self._get_read_aux(n_list, t_list)
                n_set = set(n_list)
                aux_set = set(aux)
                if len(aux_set.difference(n_set)) > 0:
                    for i in aux_set.difference(n_set):
                        self.logger.warning("get_read unknown aux name: {}".format(i))
                        aux_dic.update({i: None})

            else:
                self.logger.debug("get_read aux type unknown, accepts str or list: {}".format(aux))

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
        # This could be handled by numpy.fromiter() or similar
        # Probably MUCH faster
        # https://stackoverflow.com/questions/7543675/how-to-convert-pointer-to-c-array-to-python-array
        dic['signal'] = [self.rec.raw_signal[i] for i in range(self.rec.len_raw_signal)]
        # if pA=True, convert signal to pA
        if pA:
            dic['signal'] = self._convert_to_pA(dic)

        if aux_dic:
            dic.update(aux_dic)
        return dic


    def get_read(self, read_id, pA=False, aux=None):
        return self._get_read(read_id, pA, aux)


    def seq_reads(self, pA=False):
        '''
        returns generator for sequential reading of slow5 file
        if pa=True, do pA conversion of signal here.
        TODO: Add aux=None, take single, list, "all" for aux fields
        '''
        ret = 0
        while ret == 0:
            ret = slow5_get_next(&self.read, self.s5p)
            self.logger.debug("slow5_get_next return: {}".format(ret))
            # need a ret code for EOF to handle better
            # -2 is the current ret when EOF is hit, but it's not specific
            if ret != 0:
                self.logger.debug("slow5_get_next reached end of record (-2): {}".format(ret))
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


    def get_read_list(self, read_list, pA=False, aux=None):
        '''
        returns generator for sequential reading of slow5 file
        if pa=True, do pA conversion of signal here.
        '''
        for r in read_list:
            yield self._get_read(r, pA, aux)


    def get_header_names(self):
        '''
        get all header names and return list
        '''
        headers = []
        # ret = slow5_header_names(self.s5p.header)
        ret = slow5_get_hdr_keys(self.s5p.header, &self.head_len)

        self.logger.debug("slow5_get_hdr_keys head_len: {}".format(self.head_len))
        if ret == NULL:
            self.logger.debug("slow5_get_hdr_keys ret is NULL")
            return headers

        headers = [ret[i].decode() for i in range(self.head_len)]
        return headers

    def get_header_value(self, attr, read_group=0):
        '''
        get header attribute value
        '''
        a = str.encode(attr)
        ret = ''
        ret = slow5_hdr_get(a, read_group, self.s5p.header).decode()
        if not ret:
            self.logger.warning("get_header_value header value not found: {} - rg: {}".format(attr, read_group))
        return ret

    def get_aux_names(self):
        '''
        get all aux field names, returns list
        '''
        self.logger.debug("get_aux_names starting")
        aux_names = []
        ret = slow5_get_aux_names(self.s5p.header, &self.aux_len)

        self.logger.debug("get_aux_names aux_len: {}".format(self.aux_len))
        if ret == NULL:
            self.logger.warning("get_aux_names ret is NULL")
            return aux_names

        aux_names = [ret[i].decode() for i in range(self.aux_len)]
        return aux_names

    def get_aux_types(self):
        '''
        get aux attribute type
        '''
        self.logger.debug("get_aux_types starting")
        aux_types = []
        self.s5_aux_type = slow5_get_aux_types(self.s5p.header, &self.aux_len)

        self.logger.debug("get_aux_types aux_len: {}".format(self.aux_len))
        if self.s5_aux_type == NULL:
            self.logger.warning("get_aux_types self.s5_aux_type is NULL")
            return aux_types

        aux_types = [self.s5_aux_type[i] for i in range(self.aux_len)]
        return aux_types

    def _get_read_aux(self, aux_names, aux_types):
        '''
        get aux field for read
        '''
        dic = {}
        for name, atype in zip(aux_names, aux_types):
            a_name = str.encode(name)
            if atype == 0:
                self.e0 = slow5_aux_get_int8(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e0
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 1:
                self.e1 = slow5_aux_get_int16(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e1
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 2:
                self.e2 = slow5_aux_get_int32(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e2
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 3:
                self.e3 = slow5_aux_get_int64(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e3
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 4:
                self.e4 = slow5_aux_get_uint8(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e4
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 5:
                self.e5 = slow5_aux_get_uint16(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e5
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 6:
                self.e6 = slow5_aux_get_uint32(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e6
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 7:
                self.e7 = slow5_aux_get_uint64(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e7
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 8:
                self.e8 = slow5_aux_get_float(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e8
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 9:
                self.e9 = slow5_aux_get_double(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e9
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 10:
                self.e10 = slow5_aux_get_char(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e10
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 11:
                self.e11 = slow5_aux_get_int8_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e11[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 12:
                self.e12 = slow5_aux_get_int16_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e12[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 13:
                self.e13 = slow5_aux_get_int32_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e13[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 14:
                self.e14 = slow5_aux_get_int64_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e14[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 15:
                self.e15 = slow5_aux_get_uint8_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e15[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 16:
                self.e16 = slow5_aux_get_uint16_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e16[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 17:
                self.e17 = slow5_aux_get_uint32_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e17[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 18:
                self.e18 = slow5_aux_get_uint64_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e18[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 19:
                self.e19 = slow5_aux_get_float_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e19[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 20:
                self.e20 = slow5_aux_get_double_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e20[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            elif atype == 21:
                self.e21 = slow5_aux_get_string(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e21[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}".format(atype), self.aux_get_err)
                    dic[name] = None
            else:
                self.logger.debug("get_read_aux atype not known, skipping: {}".format(atype))

        return dic
