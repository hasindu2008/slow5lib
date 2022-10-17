# distutils: language = c
# cython: language_level=3
# cython: profile=True
import sys
import time
import logging
import copy
from itertools import chain
from libc.stdlib cimport malloc, free
from libc.string cimport strdup
cimport pyslow5
# Import the Python-level symbols of numpy
import numpy as np
# Import the C-level symbols of numpy
cimport numpy as np
# Numpy must be initialized. When using numpy from C or Cython you must
# _always_ do that, or you will have segfaults
np.import_array()

#
# Class Open for reading and writing slow5/blow5 files
# m attribute sets the read/write state and file extension of p sets the type
#

cdef class Open:
    cdef pyslow5.slow5_file_t *s5
    cdef pyslow5.slow5_rec_t *rec
    cdef pyslow5.slow5_rec_t *read
    cdef pyslow5.slow5_rec_t *write
    cdef pyslow5.slow5_rec_t **twrite
    cdef pyslow5.slow5_rec_t **trec
    cdef char **rid
    cdef char **rids
    cdef pyslow5.uint64_t rids_len
    cdef bint index_state
    cdef bint header_state
    cdef bint header_add_attr_state
    cdef bint close_state
    cdef pyslow5.uint64_t head_len
    cdef pyslow5.uint64_t aux_len
    cdef pyslow5.uint8_t enum_len
    cdef pyslow5.slow5_aux_type *s5_aux_type
    cdef char **s5_aux_enum
    cdef int aux_get_err
    cdef pyslow5.uint64_t aux_get_len
    cdef np.npy_intp shape_get[1]
    cdef np.npy_intp shape_seq[1]
    cdef char* p
    cdef str path
    cdef char* m
    cdef str mode
    cdef str rec_press
    cdef str sig_press
    cdef int state
    cdef int V
    cdef object logger
    cdef list aux_names
    cdef list aux_types
    cdef dict error_codes
    cdef dict slow5_press_method
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
    cdef int e11
    cdef pyslow5.int8_t *e12
    cdef pyslow5.int16_t *e13
    cdef pyslow5.int32_t *e14
    cdef pyslow5.int64_t *e15
    cdef pyslow5.uint8_t *e16
    cdef pyslow5.uint16_t *e17
    cdef pyslow5.uint32_t *e18
    cdef pyslow5.uint64_t *e19
    cdef float *e20
    cdef double *e21
    cdef char *e22 # meant to be string, not sure about this
    cdef char *channel_number
    cdef char *median_before
    cdef char *read_number
    cdef char *start_mux
    cdef char *start_time
    cdef char *end_reason
    cdef char **end_reason_labels
    cdef pyslow5.uint8_t end_reason_labels_len
    cdef char *channel_number_val
    cdef double median_before_val
    cdef pyslow5.int32_t read_number_val
    cdef pyslow5.uint8_t start_mux_val
    cdef pyslow5.uint64_t start_time_val
    cdef pyslow5.uint8_t end_reason_val
    cdef char **channel_number_val_array
    cdef double *median_before_val_array
    cdef pyslow5.int32_t *read_number_val_array
    cdef pyslow5.uint8_t *start_mux_val_array
    cdef pyslow5.uint64_t *start_time_val_array
    cdef pyslow5.uint8_t *end_reason_val_array

    cdef pyslow5.float total_time_slow5_get_next
    cdef pyslow5.float total_time_yield_reads
    cdef pyslow5.float total_single_write_time
    cdef pyslow5.float total_multi_write_signal_time
    cdef pyslow5.float total_multi_write_time

    def __cinit__(self, pathname, mode, rec_press="zlib", sig_press="svb_zd", DEBUG=0):
        # Set to default NULL type
        self.s5 = NULL
        self.rec = NULL
        self.read = NULL
        self.write = NULL
        self.twrite = NULL
        self.path = ""
        self.p = ""
        self.mode = ""
        self.m = ""
        self.rec_press = ""
        self.sig_press = ""
        # state for read/write. -1=null, 0=read, 1=write, 2=append
        self.state = -1
        self.trec = NULL
        self.rid = NULL
        self.rids = NULL
        self.rids_len = 0
        self.index_state = False
        self.header_state = False
        self.header_add_attr_state = False
        self.close_state = False
        self.s5_aux_type = NULL
        self.s5_aux_enum = NULL
        self.aux_get_err = 1
        self.aux_get_len = 0
        self.head_len = 0
        self.aux_len = 0
        self.enum_len = 0
        self.shape_seq[0] = 0
        self.shape_get[0] = 0
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
        self.e11 = 0
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
        self.e22 = NULL
        self.channel_number = strdup("channel_number")
        self.median_before = strdup("median_before")
        self.read_number = strdup("read_number")
        self.start_mux = strdup("start_mux")
        self.start_time = strdup("start_time")
        self.end_reason = strdup("end_reason")
        self.end_reason_labels = NULL
        self.end_reason_labels_len = 0
        channel_number_val = NULL
        median_before_val = -1.0
        read_number_val = -1
        start_mux_val = -1
        start_time_val = -1
        end_reason_val = -1
        channel_number_val_array = NULL
        median_before_val_array = NULL
        read_number_val_array = NULL
        start_mux_val_array = NULL
        start_time_val_array = NULL
        end_reason_val_array = NULL


        self.total_time_slow5_get_next = 0.0
        self.total_time_yield_reads = 0.0
        self.total_single_write_time = 0.0
        self.total_multi_write_signal_time = 0.0
        self.total_multi_write_time = 0.0

        # sets up logging level/verbosity
        self.V = DEBUG
        self.logger = logging.getLogger(__name__)
        if self.V == 1:
            lev = logging.DEBUG
        else:
            lev = logging.WARNING

        logging.basicConfig(format='%(asctime)s - [%(levelname)s]: %(message)s',
                            datefmt='%d-%b-%y %H:%M:%S', level=lev)

        # This should match slow5_defs.h error codes
        self.error_codes = {-1: ["SLOW5_ERR_EOF", "End Of File reached"],
                            -2: ["SLOW5_ERR_ARG", "bad argument (NULL)"],
                            -3: ["SLOW5_ERR_TRUNC", "file truncated"],
                            -4: ["SLOW5_ERR_RECPARSE", "record parsing error"],
                            -5: ["SLOW5_ERR_IO", "other file I/O error"],
                            -6: ["SLOW5_ERR_NOIDX", "index not loaded"],
                            -7: ["SLOW5_ERR_NOTFOUND", "read id not found"],
                            -8: ["SLOW5_ERR_OTH", "other error (big endian, internal error, etc.)"],
                            -9: ["SLOW5_ERR_UNK", "file format unknown"],
                            -10: ["SLOW5_ERR_MEM", "memory (re)allocation error"],
                            -11: ["SLOW5_ERR_NOAUX", "no auxiliary map"],
                            -12: ["SLOW5_ERR_NOFLD", "field not found"],
                            -13: ["SLOW5_ERR_PRESS", "(de)compression failure"],
                            -14: ["SLOW5_ERR_MAGIC", "magic number invalid"],
                            -15: ["SLOW5_ERR_VERSION", "version incompatible"],
                            -16: ["SLOW5_ERR_HDRPARSE", "header parsing error"],
                            -17: ["SLOW5_ERR_TYPE", "error relating to slow5 type"]}
        # slow5_press_method from slow5_press.h
        # enum slow5_press_method {
        #     SLOW5_COMPRESS_NONE,
        #     SLOW5_COMPRESS_ZLIB,
        #     SLOW5_COMPRESS_SVB_ZD, /* streamvbyte zigzag delta */
        #     SLOW5_COMPRESS_ZSTD,
        # };
        self.slow5_press_method = {"none": 0,
                                   "zlib": 1,
                                   "svb_zd": 2,
                                   "zstd": 3}

        p = str.encode(pathname)
        self.path = pathname
        self.p = strdup(p)
        m = str.encode(mode)
        self.mode = mode
        self.m = strdup(m)
        self.logger.debug("FILE: {}, mode: {}".format(self.path, self.mode))
        self.logger.debug("FILE: {}, mode: {}".format(self.p, self.m))
        # Set state based on mode for file opening
        # state for read/write. -1=null, 0=read, 1=write, 2=append
        if mode == "r":
            self.state = 0
        elif mode == "w":
            self.state = 1
            self.rec_press = rec_press
            self.sig_press = sig_press
        elif mode == "a":
            self.state = 2
        else:
            self.state = -1
        # opens file and creates slow5 object for reading
        if self.state == 0:
            self.s5 = pyslow5.slow5_open(self.p, self.m)
            self.logger.debug("Number of read_groups: {}".format(self.s5.header.num_read_groups))
        elif self.state == 1:
            self.s5 = pyslow5.slow5_open(self.p, self.m)
            if self.s5 is NULL:
                self.logger.error("File '{}' could not be opened for writing.".format(self.path))
            if "blow5" in self.path.split(".")[-1]:
                if self.rec_press in self.slow5_press_method.keys() and self.sig_press in self.slow5_press_method.keys():
                    ret = pyslow5.slow5_set_press(self.s5, self.slow5_press_method[self.rec_press], self.slow5_press_method[self.sig_press])
                    if ret != 0:
                        self.logger.error("slow5_set_press return not 0: {}".format(ret))
                        raise RuntimeError("Unable to set compression")
                else:
                    self.logger.error("Compression type rec_press: {}, sig_press: {} could not be found.".format(self.rec_press, self.sig_press))
                    self.logger.error("Please use only the following: {}".format(",".join(press for press in self.slow5_press_method.keys())))
                    raise KeyError
            else:
                self.logger.debug("Not writing blow5, skipping compression steps")
        elif self.state == 2:
            self.s5 = pyslow5.slow5_open(self.p, self.m)
            if self.s5 is NULL:
                self.logger.error("File '{}' could not be opened for writing - appending.".format(self.path))
        else:
            self.logger.error("File '{}' unknown open method: {}".format(self.path, self.mode))
        # check object was actually created.
        if self.s5 is NULL:
            raise MemoryError()


    def __init__(self, pathname, mode, rec_press="zlib", sig_press="svb_zd", DEBUG=0):
        self.aux_names = []
        self.aux_types = []


    def __dealloc__(self):
        if self.p is not NULL:
            free(self.p)
        if self.m is not NULL:
            free(self.m)
        if self.rec is not NULL:
            slow5_rec_free(self.rec)
        if self.read is not NULL:
            slow5_rec_free(self.read)
        if self.write is not NULL:
            slow5_rec_free(self.write)
        if self.state in [1, 2]:
            if not self.close_state:
                if self.s5 is not NULL:
                    slow5_close(self.s5)
                    self.close_state = True
                    self.logger.debug("{} closed".format(self.path))
            else:
                self.logger.debug("{} already closed".format(self.path))
        if self.state == 0:
            if not self.close_state:
                if self.s5 is not NULL:
                    slow5_idx_unload(self.s5)
                    slow5_close(self.s5)
                    self.close_state = True
                    self.logger.debug("{} closed".format(self.path))

        free(self.channel_number)
        free(self.median_before)
        free(self.read_number)
        free(self.start_mux)
        free(self.start_time)
        free(self.end_reason)
        if self.end_reason_labels is not NULL:
            for i in range(self.end_reason_labels_len):
                free(self.end_reason_labels[i])
            free(self.end_reason_labels)

        self.logger.debug("pathname: {}".format(self.path))
        self.logger.debug("total_time_slow5_get_next: {} seconds".format(self.total_time_slow5_get_next))
        self.logger.debug("total_time_yield_reads: {} seconds".format(self.total_time_yield_reads))
        self.logger.debug("total_single_write_time: {} seconds".format(self.total_single_write_time))
        self.logger.debug("total_multi_write_signal_time: {} seconds".format(self.total_multi_write_signal_time))
        self.logger.debug("total_multi_write_time: {} seconds".format(self.total_multi_write_time))


    def _convert_to_pA(self, d):
        '''
        convert raw signal data to pA using digitisation, offset, and range
        float raw_unit = range / digitisation;
        for (int32_t j = 0; j < nsample; j++) {
            rawptr[j] = (rawptr[j] + offset) * raw_unit;
        }
        TODO: Use memory view for the loop so it's faster
        '''
        digitisation = d['digitisation']
        range = d['range']
        offset = d['offset']
        raw_unit = range / digitisation
        # new_raw = []
        # for i in d['signal']:
        #     j = (i + offset) * raw_unit
        #     new_raw.append(float("{0:.2f}".format(round(j,2))))
        # new_raw = np.array(raw_unit * (d['signal'] + offset), dtype=np.float32)
        new_raw = (raw_unit * (d['signal'] + offset)).astype(np.float32)
        return new_raw

    # ==========================================================================
    #      Read SLOW5 file
    # ==========================================================================

    def _get_read(self, read_id, pA, aux):
        '''
        read_id = readID of individual read as a string, get's converted to b'str'
        pA = Bool for converting signal to picoamps
        aux = str 'name'/'all' or list of names of auxiliary fields added to return dictionary
        returns dic = dictionary of main fields for read_id, with any aux fields added
        '''
        if not self.index_state:
            self.logger.debug("FILE: {}, mode: {}".format(self.path, self.mode))
            # self.logger.debug("FILE: {}, mode: {}".format(self.path, self.m))
            self.logger.debug("Creating/loading index...")
            ret = slow5_idx_load(self.s5)
            if ret != 0:
                self.logger.warning("slow5_idx_load return not 0: {}: {}".format(ret, self.error_codes[ret]))
            else:
                self.index_state = True
        dic = {}
        aux_dic = {}
        ID = str.encode(read_id)
        # rec = NULL
        ret = slow5_get(ID, &self.rec, self.s5)
        if ret != 0:
            self.logger.debug("get_read return not 0: {}".format(ret))
            return None

        # check for aux fields
        if aux is not None:
            if not self.aux_names or not self.aux_types:
                self.aux_names = self.get_aux_names()
                self.aux_types = self.get_aux_types()
            if type(aux) is str:
                # special type 'all'
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
                # Check for items given that did not exist
                n_set = set(n_list)
                aux_set = set(aux)
                if len(aux_set.difference(n_set)) > 0:
                    for i in aux_set.difference(n_set):
                        self.logger.warning("get_read unknown aux name: {}".format(i))
                        aux_dic.update({i: None})

            else:
                self.logger.debug("get_read aux type unknown, accepts str or list: {}".format(aux))

        # get read data
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
        # https://groups.google.com/g/cython-users/c/KnjF7ViaHUM
        # dic['signal'] = [self.rec.raw_signal[i] for i in range(self.rec.len_raw_signal)]
        # cdef np.npy_intp shape_get[1]
        self.shape_get[0] = <np.npy_intp> self.rec.len_raw_signal
        signal = copy.deepcopy(np.PyArray_SimpleNewFromData(1, self.shape_get,
                    np.NPY_INT16, <void *> self.rec.raw_signal))
        np.PyArray_UpdateFlags(signal, signal.flags.num | np.NPY_OWNDATA)
        dic['signal'] = signal

        # if pA=True, convert signal to pA
        if pA:
            dic['signal'] = self._convert_to_pA(dic)
        # if aux data, add to main dic
        if aux_dic:
            dic.update(aux_dic)
        return dic

    def _get_batches(self, read_list, size):
        """
        batchify readIDs
        """
        for i in range(0, len(read_list), size):
            yield read_list[i:i+size]

    def _get_read_multi(self, read_list, threads, batchsize, pA, aux):
        '''
        read_list = list of readIDs
        pA = Bool for converting signal to picoamps
        aux = str 'name'/'all' or list of names of auxiliary fields added to return dictionary
        returns dic = dictionary of main fields for read_id, with any aux fields added
        threads = how many threads to use to go fast
        '''
        if not self.index_state:
            self.logger.debug("FILE: {}, mode: {}".format(self.path, self.mode))
            self.logger.debug("Creating/loading index...")
            ret = slow5_idx_load(self.s5)
            if ret != 0:
                self.logger.warning("slow5_idx_load return not 0: {}: {}".format(ret, self.error_codes[ret]))
            else:
                self.index_state = True

        self.logger.debug("Setting up batching...")
        self.logger.debug("read_list: {}".format([i for i in read_list]))
        num_reads = len(read_list)
        if num_reads > batchsize:
            batches = self._get_batches(read_list, size=batchsize)
        else:
            batches = self._get_batches(read_list, size=num_reads)

        self.logger.debug("batch for loop start")
        for batch in chain(batches):
            self.logger.debug("Batch: {}".format(",".join([i for i in batch])))
            batch_len = len(batch)
            self.logger.debug("Starting rid assignment")
            self.rid = <char **> malloc(sizeof(char*)*batch_len)
            for i in range(batch_len):
                self.logger.debug("readID: {}, bin: {}".format(batch[i], batch[i].encode()))
                self.rid[i] = strdup(batch[i].encode())
            self.logger.debug("rid assignment complete")


            self.logger.debug("slow5_get_batch: num_reads: {}".format(batch_len))
            ret = slow5_get_batch_lazy(&self.trec, self.s5, self.rid, batch_len, threads);
            self.logger.debug("get_read_multi slow5_get_batch ret: {}".format(ret))
            if ret < 0:
                self.logger.error("slow5_get_next error code: {}: {}".format(ret, self.error_codes[ret]))
                break
            if ret == 0:
                self.logger.debug("No more reads: {}".format(ret))
                break

            for i in range(ret):
                python_parse_read_start = time.time()
                self.rec = self.trec[i]
                dic = {}
                aux_dic = {}

                # check for aux fields
                if aux is not None:
                    if not self.aux_names or not self.aux_types:
                        self.aux_names = self.get_aux_names()
                        self.aux_types = self.get_aux_types()
                    if type(aux) is str:
                        # special type 'all'
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
                        # Check for items given that did not exist
                        n_set = set(n_list)
                        aux_set = set(aux)
                        if len(aux_set.difference(n_set)) > 0:
                            for i in aux_set.difference(n_set):
                                self.logger.warning("get_read unknown aux name: {}".format(i))
                                aux_dic.update({i: None})

                    else:
                        self.logger.debug("get_read aux type unknown, accepts str or list: {}".format(aux))

                # get read data
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
                # https://groups.google.com/g/cython-users/c/KnjF7ViaHUM
                # dic['signal'] = [self.rec.raw_signal[i] for i in range(self.rec.len_raw_signal)]
                # cdef np.npy_intp shape_get[1]
                self.shape_get[0] = <np.npy_intp> self.rec.len_raw_signal
                signal = copy.deepcopy(np.PyArray_SimpleNewFromData(1, self.shape_get,
                            np.NPY_INT16, <void *> self.rec.raw_signal))
                np.PyArray_UpdateFlags(signal, signal.flags.num | np.NPY_OWNDATA)
                dic['signal'] = signal

                # if pA=True, convert signal to pA
                if pA:
                    dic['signal'] = self._convert_to_pA(dic)
                # if aux data, add to main dic
                if aux_dic:
                    dic.update(aux_dic)
                yield dic

            slow5_free_batch_lazy(&self.trec, ret)
            for i in range(batch_len):
                free(self.rid[i])
            free(self.rid)
        self.rec = NULL
        self.logger.debug("seq_reads_multi timings:")


    def _get_read_aux(self, aux_names, aux_types):
        '''
        get aux field for read
        aux_names and aux_types need to be lists, even if a single element
        both names and types need to be in the same order, to ensure correct type function is called
        This ensures correct C -> python object conversion
        Returns dictionary with dic[name] = value
        '''
        dic = {}
        # treat all input as list to simplify
        for name, atype in zip(aux_names, aux_types):
            a_name = str.encode(name)
            if atype == 0:
                self.e0 = slow5_aux_get_int8(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e0
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 1:
                self.e1 = slow5_aux_get_int16(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e1
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 2:
                self.e2 = slow5_aux_get_int32(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e2
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 3:
                self.e3 = slow5_aux_get_int64(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e3
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 4:
                self.e4 = slow5_aux_get_uint8(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e4
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 5:
                self.e5 = slow5_aux_get_uint16(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e5
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 6:
                self.e6 = slow5_aux_get_uint32(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e6
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 7:
                self.e7 = slow5_aux_get_uint64(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e7
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 8:
                self.e8 = slow5_aux_get_float(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e8
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 9:
                self.e9 = slow5_aux_get_double(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e9
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 10:
                self.e10 = slow5_aux_get_char(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e10
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 11:
                self.e11 = slow5_aux_get_enum(self.rec, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e11
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype), self.aux_get_err, self.error_codes[self.aux_get_err])
                    dic[name] = None
            elif atype == 12:
                self.e12 = slow5_aux_get_int8_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e12[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 13:
                self.e13 = slow5_aux_get_int16_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e13[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 14:
                self.e14 = slow5_aux_get_int32_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e14[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 15:
                self.e15 = slow5_aux_get_int64_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e15[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 16:
                self.e16 = slow5_aux_get_uint8_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e16[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 17:
                self.e17 = slow5_aux_get_uint16_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e17[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 18:
                self.e18 = slow5_aux_get_uint32_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e18[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 19:
                self.e19 = slow5_aux_get_uint64_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e19[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 20:
                self.e20 = slow5_aux_get_float_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e20[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 21:
                self.e21 = slow5_aux_get_double_array(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e21[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 22:
                self.e22 = slow5_aux_get_string(self.rec, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    s = ""
                    for i in range(self.aux_get_len):
                        l.append(chr(self.e22[i]))
                        s = "".join(l)
                    dic[name] = s
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 22:
                self.logger.debug("NOT IMPLEMENTED YET: get_aux_types {} self.aux_get_err is {}: {}".format(atype), self.aux_get_err, self.error_codes[self.aux_get_err])
                dic[name] = None
            else:
                self.logger.debug("get_read_aux atype not known, skipping: {}".format(atype))

        return dic


    def _get_seq_read_aux(self, aux_names, aux_types):
        '''
        get aux field for read - when doing sequential read, because of separate self.read (vs rec) call
        aux_names and aux_types need to be lists, even if a single element
        both names and types need to be in the same order, to ensure correct type function is called
        This ensures correct C -> python object conversion
        Returns dictionary with dic[name] = value
        '''
        dic = {}
        # treat all input as a list to simplify
        for name, atype in zip(aux_names, aux_types):
            a_name = str.encode(name)
            if atype == 0:
                self.e0 = slow5_aux_get_int8(self.read, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e0
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 1:
                self.e1 = slow5_aux_get_int16(self.read, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e1
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 2:
                self.e2 = slow5_aux_get_int32(self.read, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e2
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 3:
                self.e3 = slow5_aux_get_int64(self.read, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e3
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 4:
                self.e4 = slow5_aux_get_uint8(self.read, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e4
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 5:
                self.e5 = slow5_aux_get_uint16(self.read, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e5
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 6:
                self.e6 = slow5_aux_get_uint32(self.read, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e6
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 7:
                self.e7 = slow5_aux_get_uint64(self.read, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e7
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 8:
                self.e8 = slow5_aux_get_float(self.read, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e8
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 9:
                self.e9 = slow5_aux_get_double(self.read, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e9
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 10:
                self.e10 = slow5_aux_get_char(self.read, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e10
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 11:
                self.e11 = slow5_aux_get_enum(self.read, a_name, &self.aux_get_err)
                if self.aux_get_err == 0:
                    dic[name] = self.e11
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype), self.aux_get_err, self.error_codes[self.aux_get_err])
                    dic[name] = None
            elif atype == 12:
                self.e12 = slow5_aux_get_int8_array(self.read, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e12[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 13:
                self.e13 = slow5_aux_get_int16_array(self.read, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e13[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 14:
                self.e14 = slow5_aux_get_int32_array(self.read, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e14[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 15:
                self.e15 = slow5_aux_get_int64_array(self.read, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e15[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 16:
                self.e16 = slow5_aux_get_uint8_array(self.read, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e16[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 17:
                self.e17 = slow5_aux_get_uint16_array(self.read, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e17[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 18:
                self.e18 = slow5_aux_get_uint32_array(self.read, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e18[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 19:
                self.e19 = slow5_aux_get_uint64_array(self.read, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e19[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 20:
                self.e20 = slow5_aux_get_float_array(self.read, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e20[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 21:
                self.e21 = slow5_aux_get_double_array(self.read, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    for i in range(self.aux_get_len):
                        l.append(self.e21[i])
                    dic[name] = l
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 22:
                self.e22 = slow5_aux_get_string(self.read, a_name, &self.aux_get_len, &self.aux_get_err)
                if self.aux_get_err == 0:
                    l = []
                    s = ""
                    for i in range(self.aux_get_len):
                        l.append(chr(self.e22[i]))
                        s = "".join(l)
                    dic[name] = s
                else:
                    self.logger.debug("get_aux_types {} self.aux_get_err is {}: {}".format(atype, self.aux_get_err, self.error_codes[self.aux_get_err]))
                    dic[name] = None
            elif atype == 22:
                self.logger.debug("NOT IMPLEMENTED YET: get_aux_types {} self.aux_get_err is {}: {}".format(atype), self.aux_get_err, self.error_codes[self.aux_get_err])
                dic[name] = None
            else:
                self.logger.debug("get_read_aux atype not known, skipping: {}".format(atype))

        return dic


    def get_read_ids(self):
        '''
        get all read_ids from index
        if no index, build it then get read_ids
        No idea why this is needed but whatever
        '''
        rids = []
        if not self.index_state:
            self.logger.debug("FILE: {}, mode: {}".format(self.path, self.mode))
            # self.logger.debug("FILE: {}, mode: {}".format(self.path, self.m))
            self.logger.debug("Creating/loading index...")
            ret = slow5_idx_load(self.s5)
            if ret != 0:
                self.logger.warning("slow5_idx_load return not 0: {}: {}".format(ret, self.error_codes[ret]))
            else:
                self.index_state = True
        self.rids = slow5_get_rids(self.s5, &self.rids_len)

        if self.rids_len == 0:
            self.logger.error("get_read_ids: length of read_ids is 0, something when wrong")

        if self.rids is NULL:
            self.logger.error("get_read_ids: returned rids is NULL, something when wrong")
        else:
            for i in range(self.rids_len):
                rids.append(self.rids[i].decode())
        rids_len = self.rids_len
        return rids, rids_len


    def get_read(self, read_id, pA=False, aux=None):
        return self._get_read(read_id, pA, aux)


    def seq_reads(self, pA=False, aux=None):
        '''
        returns generator for sequential reading of slow5 file
        for pA and aux, see _get_read
        '''
        aux_dic = {}
        row = {}
        ret = 0
        # While loops check ret of previous read for errors as fail safe
        while ret >= 0:
            start_slow5_get_next = time.time()
            ret = slow5_get_next(&self.read, self.s5)
            self.total_time_slow5_get_next = self.total_time_slow5_get_next + (time.time() - start_slow5_get_next)

            self.logger.debug("slow5_get_next return: {}".format(ret))
            # check for EOF or other errors
            if ret < 0:
                if ret == -1:
                    self.logger.debug("slow5_get_next reached end of file (EOF)(-1): {}: {}".format(ret, self.error_codes[ret]))
                else:
                    self.logger.error("slow5_get_next error code: {}: {}".format(ret, self.error_codes[ret]))

                break

            aux_dic = {}
            row = {}
            # get aux fields
            if aux is not None:
                if not self.aux_names or not self.aux_types:
                    self.aux_names = self.get_aux_names()
                    self.aux_types = self.get_aux_types()
                if type(aux) is str:
                    if aux == "all":
                        aux_dic = self._get_seq_read_aux(self.aux_names, self.aux_types)
                    else:
                        found_single_aux = False
                        for n, t in zip(self.aux_names, self.aux_types):
                            if n == aux:
                                found_single_aux = True
                                aux_dic = self._get_seq_read_aux([n], [t])
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

                    aux_dic = self._get_seq_read_aux(n_list, t_list)
                    # check for items in given list that do not exist
                    n_set = set(n_list)
                    aux_set = set(aux)
                    if len(aux_set.difference(n_set)) > 0:
                        for i in aux_set.difference(n_set):
                            self.logger.warning("get_read unknown aux name: {}".format(i))
                            aux_dic.update({i: None})

                else:
                    self.logger.debug("get_read aux type unknown, accepts str or list: {}".format(aux))

            # Get read data
            if type(self.read.read_id) is bytes:
                row['read_id'] = self.read.read_id.decode()
            else:
                row['read_id'] = self.read.read_id
            row['read_group'] = self.read.read_group
            row['digitisation'] = self.read.digitisation
            row['offset'] = self.read.offset
            row['range'] = self.read.range
            row['sampling_rate'] = self.read.sampling_rate
            row['len_raw_signal'] = self.read.len_raw_signal
            self.shape_seq[0] = <np.npy_intp> self.read.len_raw_signal
            signal = copy.deepcopy(np.PyArray_SimpleNewFromData(1, self.shape_seq,
                        np.NPY_INT16, <void *> self.read.raw_signal))
            np.PyArray_UpdateFlags(signal, signal.flags.num | np.NPY_OWNDATA)
            row['signal'] = signal

            # if pA=True, convert signal to pA
            if pA:
                row['signal'] = self._convert_to_pA(row)
            # if aux data update main dic
            if aux_dic:
                row.update(aux_dic)
            self.total_time_yield_reads = self.total_time_yield_reads + (time.time() - start_slow5_get_next)
            yield row


    def seq_reads_multi(self, threads=4, batchsize=4096, pA=False, aux=None):
        '''
        returns generator for sequential reading of slow5 file
        for pA and aux, see _get_read
        threads: number of threads to use
        batchsize: Number of reads to process with thread pool in parallel
        '''
        aux_dic = {}
        row = {}
        ret = 1
        timedic = {"aux_total_time": 0,
                   "primary_total_time": 0,
                   "pA_total_time": 0,
                   "signal_total_time": 0}

        # While loops check ret of previous read for errors as fail safe
        while ret > 0:
            start_slow5_get_next = time.time()
            ret = slow5_get_next_batch_lazy(&self.trec, self.s5, batchsize, threads)
            self.total_time_slow5_get_next = self.total_time_slow5_get_next + (time.time() - start_slow5_get_next)
            self.logger.debug("slow5_get_next_multi return: {}".format(ret))
            # check for EOF or other errors
            if ret < 0:
                if ret == -1:
                    self.logger.debug("slow5_get_next_multi reached end of file (EOF)(-1): {}: {}".format(ret, self.error_codes[ret]))
                else:
                    self.logger.error("slow5_get_next_multi error code: {}: {}".format(ret, self.error_codes[ret]))

                break
            for i in range(ret):
                python_parse_read_start = time.time()
                self.read = self.trec[i]
                aux_dic = {}
                row = {}
                # get aux fields
                aux_time_start = time.time()
                if aux is not None:
                    if not self.aux_names or not self.aux_types:
                        self.aux_names = self.get_aux_names()
                        self.aux_types = self.get_aux_types()
                    if type(aux) is str:
                        if aux == "all":
                            aux_dic = self._get_seq_read_aux(self.aux_names, self.aux_types)
                        else:
                            found_single_aux = False
                            for n, t in zip(self.aux_names, self.aux_types):
                                if n == aux:
                                    found_single_aux = True
                                    aux_dic = self._get_seq_read_aux([n], [t])
                                    break
                            if not found_single_aux:
                                self.logger.warning("slow5_get_next_multi unknown aux name: {}".format(aux))
                                aux_dic.update({aux: None})
                    elif type(aux) is list:
                        n_list = []
                        t_list = []
                        for n, t in zip(self.aux_names, self.aux_types):
                            if n in aux:
                                n_list.append(n)
                                t_list.append(t)

                        aux_dic = self._get_seq_read_aux(n_list, t_list)
                        # check for items in given list that do not exist
                        n_set = set(n_list)
                        aux_set = set(aux)
                        if len(aux_set.difference(n_set)) > 0:
                            for i in aux_set.difference(n_set):
                                self.logger.warning("slow5_get_next_multi unknown aux name: {}".format(i))
                                aux_dic.update({i: None})

                    else:
                        self.logger.debug("slow5_get_next_multi aux type unknown, accepts str or list: {}".format(aux))
                timedic["aux_total_time"] = timedic["aux_total_time"] + (time.time() - aux_time_start)
                # Get read data
                primary_start_time = time.time()
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
                signal_start_time = time.time()
                self.shape_seq[0] = <np.npy_intp> self.read.len_raw_signal
                signal = copy.deepcopy(np.PyArray_SimpleNewFromData(1, self.shape_seq,
                            np.NPY_INT16, <void *> self.read.raw_signal))
                np.PyArray_UpdateFlags(signal, signal.flags.num | np.NPY_OWNDATA)
                row['signal'] = signal
                timedic["signal_total_time"] = timedic["signal_total_time"] + (time.time() - signal_start_time)
                timedic["primary_total_time"] = timedic["primary_total_time"] + (time.time() - primary_start_time)
                # if pA=True, convert signal to pA
                if pA:
                    pA_start_time = time.time()
                    row['signal'] = self._convert_to_pA(row)
                    timedic["pA_total_time"] = timedic["pA_total_time"] + (time.time() - pA_start_time)
                # if aux data update main dic
                if aux_dic:
                    row.update(aux_dic)
                self.total_time_yield_reads = self.total_time_yield_reads + (time.time() - python_parse_read_start)
                yield row
            slow5_free_batch_lazy(&self.trec, ret)
            if ret < batchsize:
                self.logger.debug("slow5_get_next_multi has no more batches - batchsize:{} ret:{}".format(batchsize, ret))
                break
        self.read = NULL
        self.logger.debug("seq_reads_multi timings:")
        for i in timedic:
            self.logger.debug("{}: {}".format(i, timedic[i]))


    def get_read_list(self, read_list, pA=False, aux=None):
        '''
        returns generator for random access of slow5 file
        read_list = list of readIDs, if readID not in file, None type returned (need EOF to work)
        for pA and aux see _get_read
        '''
        for r in read_list:
            yield self._get_read(r, pA, aux)


    def get_read_list_multi(self, read_list, threads=4, batchsize=100, pA=False, aux=None):
        '''
        returns generator for random access of slow5 file
        read_list = list of readIDs, if readID not in file, None type returned (need EOF to work)
        for pA and aux see _get_read
        threads = number of threads to use to do batched random access
        '''
        for r in self._get_read_multi(read_list, threads, batchsize, pA, aux):
            yield r


    def get_header_names(self):
        '''
        get all header names and return list
        '''
        headers = []
        ret = slow5_get_hdr_keys(self.s5.header, &self.head_len)

        self.logger.debug("slow5_get_hdr_keys head_len: {}".format(self.head_len))
        if ret == NULL:
            self.logger.debug("slow5_get_hdr_keys ret is NULL")
            return headers

        headers = [ret[i].decode() for i in range(self.head_len)]
        free(ret)
        return headers


    def get_header_value(self, attr, read_group=0):
        '''
        get header attribute value
        '''
        a = str.encode(attr)
        ret = ''
        ret = slow5_hdr_get(a, read_group, self.s5.header).decode()
        if not ret:
            self.logger.warning("get_header_value header value not found: {} - rg: {}".format(attr, read_group))
            return None
        return ret


    def get_all_headers(self, read_group=0):
        '''
        get all headers present and return dictionary
        '''
        dic = {}
        headers = self.get_header_names()
        for header in headers:
            val = self.get_header_value(header, read_group=read_group)
            dic[header] = val
        return dic


    def get_aux_names(self):
        '''
        get all aux field names, returns list
        '''
        self.logger.debug("get_aux_names starting")
        aux_names = []
        ret = slow5_get_aux_names(self.s5.header, &self.aux_len)

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
        self.s5_aux_type = slow5_get_aux_types(self.s5.header, &self.aux_len)

        self.logger.debug("get_aux_types aux_len: {}".format(self.aux_len))
        if self.s5_aux_type == NULL:
            self.logger.warning("get_aux_types self.s5_aux_type is NULL")
            return aux_types

        aux_types = [self.s5_aux_type[i] for i in range(self.aux_len)]
        return aux_types


    def get_aux_enum_labels(self, label):
        '''
        get the labels for an enum aux field
        '''
        a = str.encode(label)
        labels = []
        self.s5_aux_enum = slow5_get_aux_enum_labels(self.s5.header, a, &self.enum_len)

        if self.s5_aux_enum == NULL:
            self.logger.warning("get_aux_enum_labels enum_labels is NULL")
            return labels

        labels = [self.s5_aux_enum[i].decode() for i in range(self.enum_len)]
        return labels


    # ==========================================================================
    #      Write SLOW5 file
    # ==========================================================================

    def get_empty_header(self, aux=False):
        '''
        returns example empty header dic for user to populate
        Any values not populated will be skipped
        '''
        header = {"asic_id": None,
                  "asic_id_eeprom": None,
                  "asic_temp": None,
                  "asic_version": None,
                  "auto_update": None,
                  "auto_update_source": None,
                  "barcoding_enabled": None,
                  "bream_is_standard": None,
                  "configuration_version": None,
                  "device_id": None,
                  "device_type": None,
                  "distribution_status": None,
                  "distribution_version": None,
                  "exp_script_name": None,
                  "exp_script_purpose": None,
                  "exp_start_time": None,
                  "experiment_duration_set": None,
                  "flow_cell_id": None,
                  "flow_cell_product_code": None,
                  "guppy_version": None,
                  "heatsink_temp": None,
                  "hostname": None,
                  "installation_type": None,
                  "local_basecalling": None,
                  "operating_system": None,
                  "package": None,
                  "protocol_group_id": None,
                  "protocol_run_id": None,
                  "protocol_start_time": None,
                  "protocols_version": None,
                  "run_id": None,
                  "sample_frequency": None,
                  "sample_id": None,
                  "sequencing_kit": None,
                  "usb_config": None,
                  "version": None,
                  "hublett_board_id": None,
                  "satellite_firmware_version": None}

        end_reason_labels = ['unknown', 'partial', 'mux_change', 'unblock_mux_change', 'signal_positive', 'signal_negative']

        if aux:
            return header, end_reason_labels
        return header

    def get_empty_record(self, aux=False):
        '''
        Return a dictionary of empty record and types
        if axu=True, a second dictionary will be returned with optional aux record
        '''
        record = {"read_id": None,
                  "read_group": 0,
                  "digitisation": None,
                  "offset": None,
                  "range": None,
                  "sampling_rate": None,
                  "len_raw_signal": None,
                  "signal": None}

        aux_rec = {"channel_number": None,
               "median_before": None,
               "read_number": None,
               "start_mux": None,
               "start_time": None,
               "end_reason": None}

        if aux:
            return record, aux_rec
        return record


    def _header_type_validation(self, header):
        '''
        internal function to validate header types before pushing to C side
        '''
        for h in header:
            if header[h] is not None:
                t = type(header[h])
                if t is not type("string"):
                    self.logger.warning("_header_type_validation {}: {} is not a string type, attempting to convert".format(h, header[h]))
                    #try and convert
                    try:
                        s = str(header[h])
                        header[h] = s
                        self.logger.warning("_header_type_validation {}: {} conversion successful".format(h, s))
                    except:
                        self.logger.error("_header_type_validation {}: {} could not convert value to string".format(h, header[h]))
                        raise
        return header


    def _aux_header_type_validation(self, user_aux_types):
        '''
        internal function to validate and convert aux types before pushing to C side
        '''
        slow5_aux_types = {}

        py_aux_types = {"channel_number": type("string"),
                     "median_before": type(1.0),
                     "read_number": type(10),
                     "start_mux": type(1),
                     "start_time": type(100),
                     "end_reason": type(["a", "b", "c"])}

        C_aux_types = {"channel_number": SLOW5_STRING,
                     "median_before":SLOW5_DOUBLE,
                     "read_number": SLOW5_INT32_T,
                     "start_mux": SLOW5_UINT8_T,
                     "start_time": SLOW5_UINT64_T,
                     "end_reason": SLOW5_ENUM}

        for a in user_aux_types:
            if user_aux_types[a] is None:
                continue
            if a not in py_aux_types:
                self.logger.error("_aux_type_validation {}: {} user aux field not in pyslow5 aux_types".format(a, user_aux_types[a]))
                return None
            if user_aux_types[a] == py_aux_types[a]:
                slow5_aux_types[a] = C_aux_types[a]
            else:
                self.logger.error("_aux_type_validation {}: {} user aux field type mismatch with pyslow5 aux_types".format(a, user_aux_types[a]))
                return None
        return slow5_aux_types


    def _record_type_validation(self, user_record, aux=None):
        '''
        internal function to validate and convert aux types before pushing to C side
        returns None, None on error
        '''

        py_record_types = {"read_id": type("string"),
                           "read_group": type(1),
                           "digitisation": type(1.0),
                           "offset": type(1.0),
                           "range": type(1.0),
                           "sampling_rate": type(1.0),
                           "len_raw_signal": type(10),
                           "signal": type(np.array([1, 2, 3], np.int16))}

        py_aux_types = {"channel_number": type("string"),
                        "median_before": type(1.0),
                        "read_number": type(10),
                        "start_mux": type(1),
                        "start_time": type(100),
                        "end_reason": type(1)}

        new_aux = {}

        for a in user_record:
            if user_record[a] is None:
                continue
            if a not in py_record_types:
                self.logger.error("_record_type_validation {}: {} user primary field not in pyslow5 record_types".format(a, user_record[a]))
                return None, None

            if type(user_record[a]) != py_record_types[a]:
                self.logger.error("_record_type_validation {}: {} user primary field type mismatch with pyslow5 record_types".format(a, user_record[a]))
                return None, None

        # check aux if given
        if aux is not None:
            self.logger.debug("_record_type_validation: doing aux stuff...")
            for a in aux:
                if a not in py_aux_types:
                    self.logger.error("_record_type_validation {}: {} user aux field not in pyslow5 aux_types".format(a, aux[a]))
                    return None, None
                if aux[a] is None:
                    continue
                if type(aux[a]) != py_aux_types[a]:
                    self.logger.error("_record_type_validation {}: {} user aux field type mismatch with pyslow5 aux_types".format(a, aux[a]))
                    return None, None
                else:
                    self.logger.debug("_record_type_validation: aux passed tests...")
                    if a == "channel_number":
                        self.channel_number_val=strdup(aux[a].encode())
                        new_aux[a] = aux[a]
                    elif a == "median_before":
                        self.median_before_val = <double>aux[a]
                        new_aux[a] = aux[a]
                    elif a == "read_number":
                        self.read_number_val = <int32_t>aux[a]
                        new_aux[a] = aux[a]
                    elif a == "start_mux":
                        self.start_mux_val = <uint8_t>aux[a]
                        new_aux[a] = aux[a]
                    elif a == "start_time":
                        self.start_time_val = <uint64_t>aux[a]
                        new_aux[a] = aux[a]
                    elif a == "end_reason":
                        self.end_reason_val = <uint8_t>aux[a]
                        new_aux[a] = aux[a]
                    else:
                         self.logger.error("_record_type_validation {}: {} user aux field unknown?".format(a, aux[a]))


        return user_record, new_aux


    def _multi_record_type_validation(self, user_record, aux=None):
        '''
        internal function to validate and convert aux types before pushing to C side
        returns None, None on error
        '''

        py_record_types = {"read_id": type("string"),
                           "read_group": type(1),
                           "digitisation": type(1.0),
                           "offset": type(1.0),
                           "range": type(1.0),
                           "sampling_rate": type(1.0),
                           "len_raw_signal": type(10),
                           "signal": type(np.array([1, 2, 3], np.int16))}

        py_aux_types = {"channel_number": type("string"),
                        "median_before": type(1.0),
                        "read_number": type(10),
                        "start_mux": type(1),
                        "start_time": type(100),
                        "end_reason": type(1)}

        new_aux = {}

        for a in user_record:
            if user_record[a] is None:
                continue
            if a not in py_record_types:
                self.logger.error("_record_type_validation {}: {} user primary field not in pyslow5 record_types".format(a, user_record[a]))
                return None, None

            if type(user_record[a]) != py_record_types[a]:
                self.logger.error("_record_type_validation {}: {} user primary field type mismatch with pyslow5 record_types".format(a, user_record[a]))
                return None, None

        # check aux if given
        if aux is not None:
            self.logger.debug("_record_type_validation: doing aux stuff...")
            for a in aux:
                if a not in py_aux_types:
                    self.logger.error("_record_type_validation {}: {} user aux field not in pyslow5 aux_types".format(a, aux[a]))
                    return None, None
                if aux[a] is None:
                    continue
                if type(aux[a]) != py_aux_types[a]:
                    self.logger.error("_record_type_validation {}: {} user aux field type mismatch with pyslow5 aux_types".format(a, aux[a]))
                    return None, None
                else:
                    self.logger.debug("_record_type_validation: aux passed tests...")
                    if a == "channel_number":
                        new_aux[a] = aux[a]
                    elif a == "median_before":
                        new_aux[a] = aux[a]
                    elif a == "read_number":
                        new_aux[a] = aux[a]
                    elif a == "start_mux":
                        new_aux[a] = aux[a]
                    elif a == "start_time":
                        new_aux[a] = aux[a]
                    elif a == "end_reason":
                        new_aux[a] = aux[a]
                    else:
                        self.logger.error("_record_type_validation {}: {} user aux field unknown".format(a, aux[a]))

            self.logger.debug("_record_type_validation: aux stuff done")


        return user_record, new_aux


    def write_header(self, header, read_group=0, end_reason_labels=None):
        '''
        write slow5 header to file.
        takes header dic for attributes, then write once.
        Must write readgroup 0 before 1, 1 before 2, etc
        '''
        checked_header = self._header_type_validation(header)

        errors = False
        if not self.header_add_attr_state:
            if read_group > 0:
                self.logger.error("write_header: You must set read_group=0 first")
                self.logger.error("write_header: headers failed to initialise.")
                return -1
            for h in checked_header:
                ret = slow5_hdr_add_attr(h.encode(), self.s5.header)
                if ret < 0:
                    self.logger.error("write_header: slow5_hdr_add_attr {}: {} could not initialise to C s5.header struct".format(h, checked_header[h]))
                    errors = True
                else:
                    self.header_add_attr_state = True

        if read_group > 0 and self.header_add_attr_state:
            ret = slow5_hdr_add_rg(self.s5.header)
            if ret < 0:
                self.logger.error("write_header: slow5_hdr_add_rg failed for read_group: {}".format(read_group))
                errors = True

        if errors:
            self.logger.error("write_header: headers failed to initialise.")
            return -1

        for h in checked_header:
            ret = slow5_hdr_set(h.encode(), checked_header[h].encode(), read_group, self.s5.header)
            if ret < 0:
                self.logger.error("write_header: slow5_hdr_set {}: {} could not set to C s5.header struct".format(h, checked_header[h]))
                errors = True

        # check end_reason_labels type
        if end_reason_labels is not None:
            erl = []
            for i in end_reason_labels:
                t = type(i)
                if t is not type("string"):
                    self.logger.error("write_header: end_reason_labels: {} not type: string, trying to convert".format(i))
                    try:
                        s = str(i)
                        erl.append(s)
                        self.logger.warning("write_header end_reason_labels: {} conversion successful".format(s))
                    except:
                        self.logger.error("write_header end_reason_labels: {} could not convert value to string".format(i))
                        errors = True
                else:
                    erl.append(i)
            self.end_reason_labels = <char **> malloc(sizeof(char*)*len(erl))
            for i in range(len(erl)):
                self.end_reason_labels[i] = strdup(erl[i].encode())
            self.end_reason_labels_len = <uint8_t>len(erl)

        if not errors:
            return 0
        else:
            self.logger.error("write_header: errors encountered setting up header, aborting write")
            # sys.exit(1)
            return -1

    def write_record(self, record, aux=None):
        '''
        write a single record
        check self.header_state, if False, header needs to be written. Use aux types to do first check.
        if the validation works, then write the header just before the first record is written.
        This should make the user experience a lot less complicated and have less steps
        '''

        aux_types = {"channel_number": type("string"),
                     "median_before": type(1.0),
                     "read_number": type(10),
                     "start_mux": type(1),
                     "start_time": type(100),
                     "end_reason": type(["a", "b", "c"])}

        self.logger.debug("write_record: _record_type_validation running")
        checked_record, checked_aux = self._record_type_validation(record, aux)
        self.logger.debug("write_record: _record_type_validation done")

        # check if in append state
        # if so, we can skip header setup
        if self.state == 2:
            self.header_state = True

        # if all checks are good, test self.header_state for 1 time write
        if not self.header_state:
            self.logger.debug("write_record: checking header stuff...")
            error = False
            if aux is not None:
                if checked_aux is not None:
                    slow5_aux_types = self._aux_header_type_validation(aux_types)
                    for a in slow5_aux_types:
                        if a not in checked_aux:
                            continue
                        elif checked_aux[a] is None:
                            continue
                        elif slow5_aux_types[a] == SLOW5_ENUM:
                            ret = slow5_aux_add_enum(a.encode(), <const char **>self.end_reason_labels, self.end_reason_labels_len, self.s5.header)
                            if ret < 0:
                                self.logger.error("write_record: slow5_aux_add_enum {}: {} could not set to C s5.header.aux_meta struct".format(a, checked_aux[a]))
                                error = True
                        else:
                            ret = slow5_aux_add(a.encode(), slow5_aux_types[a], self.s5.header)
                            if ret < 0:
                                self.logger.error("write_record: slow5_aux_add {}: {} could not set to C s5.header.aux_meta struct".format(a, checked_aux[a]))
                                error = True
                else:
                    error = True

            if error:
                self.logger.error("write_record: aux_meta fields failed to initialise")
                return -1
            # write the header
            self.logger.debug("write_record: writting header...")
            ret = slow5_hdr_write(self.s5)
            if ret < 0:
                self.logger.error("write_record: slow5_hdr_write could not write header")
                return -1
            # set state true so only done once
            self.header_state = True
            self.logger.debug("write_record: header written")

        # Add values to read struct
        self.logger.debug("write_record: slow5_rec_init()")
        self.write = slow5_rec_init()
        if self.write == NULL:
            self.logger.error("write_record: failed to allocate space for slow5 record (self.write)")
            return -1

        self.logger.debug("write_record: self.write assignments...")
        checked_record["read_id"] = checked_record["read_id"].encode()
        self.write.read_id = strdup(checked_record["read_id"])
        self.write.read_id_len = len(checked_record["read_id"])
        self.write.read_group = checked_record["read_group"]
        self.write.digitisation = checked_record["digitisation"]
        self.write.offset = checked_record["offset"]
        self.write.range = checked_record["range"]
        self.write.sampling_rate = checked_record["sampling_rate"]
        self.write.len_raw_signal = checked_record["len_raw_signal"]
        self.logger.debug("write_record: self.write.raw_signal malloc...")
        self.write.raw_signal = <int16_t *> malloc(sizeof(int16_t)*checked_record["len_raw_signal"])
        self.logger.debug("write_record: self.write.raw_signal malloc done")
        self.logger.debug("write_record: self.write assignments done")

        self.logger.debug("write_record: self.write processing raw_signal")
        start_write_copy_signal = time.time()
        # grabs buffer of numby array so the for loop operats in C not python = super fast
        memview = memoryview(checked_record["signal"])
        for i in range(checked_record["len_raw_signal"]):
            self.write.raw_signal[i] = memview[i]
        # for i in range(checked_record["len_raw_signal"]):
        #     self.write.raw_signal[i] = checked_record["signal"][i]
        end_write_copy_signal = (time.time() - start_write_copy_signal)
        self.total_single_write_time = self.total_single_write_time + end_write_copy_signal
        self.logger.debug("write_record: self.write raw_signal done")

        if aux:
            self.logger.debug("write_record: aux stuff...")
            if checked_aux is None:
                self.logger.error("write_record: checked_aux is None".format(a, checked_aux[a]))
                return -1

            for a in checked_aux:
                self.logger.debug("write_record: checked_aux: {}: {}".format(a, checked_aux[a]))
                if checked_aux[a] is None:
                    continue
                if a == "channel_number":
                    self.logger.debug("write_record: slow5_aux_set_string running...")
                    self.logger.debug("write_record: slow5_aux_set_string type: {}".format(type(checked_aux[a])))
                    ret = slow5_aux_set_string(self.write, self.channel_number, <const char *>self.channel_number_val, self.s5.header)
                    self.logger.debug("write_record: slow5_aux_set_string running done: ret = {}".format(ret))
                    if ret < 0:
                        self.logger.error("write_record: slow5_aux_set_string could not write aux value {}: {}".format(a, checked_aux[a]))
                        #### We should free here
                        return -1

                elif a == "median_before":
                    ret = slow5_aux_set(self.write, self.median_before, <const void *>&self.median_before_val, self.s5.header)
                elif a == "read_number":
                    ret = slow5_aux_set(self.write, self.read_number, <const void *>&self.read_number_val, self.s5.header)
                elif a == "start_mux":
                    ret = slow5_aux_set(self.write, self.start_mux, <const void *>&self.start_mux_val, self.s5.header)
                elif a == "start_time":
                    ret = slow5_aux_set(self.write, self.start_time, <const void *>&self.start_time_val, self.s5.header)
                elif a == "end_reason":
                    ret = slow5_aux_set(self.write, self.end_reason, <const void *>&self.end_reason_val, self.s5.header)
                else:
                    ret = -1
                if ret < 0:
                    self.logger.error("write_record: slow5_aux_set could not write aux value {}: {}".format(a, checked_aux[a]))
                    return -1

            self.logger.debug("write_record: aux stuff done")


        self.logger.debug("write_record: slow5_write()")
        # write the record
        ret = slow5_write(self.write, self.s5)
        self.logger.debug("write_record: slow5_write() ret: {}".format(ret))

        if aux is not None:
            free(self.channel_number_val)
            self.channel_number_val = NULL
            self.median_before_val = -1.0
            self.read_number_val = -1
            self.start_mux_val = -1
            self.start_time_val = -1
            self.end_reason_val = -1


        # free memory
        self.logger.debug("write_record: slow5_rec_free()")
        slow5_rec_free(self.write)
        self.write = NULL

        self.logger.debug("write_record: function complete, returning 0")
        return 0


    def write_record_batch(self, records, threads=4, batchsize=4096, aux=None):
        '''
        write a batch of records
        same as write_record, but in batches
        records = dic of read rics, where the key for each is the readID
        if aux, aux is also a dic of dics, whhere the key for each is teh readID
        records = {readID_0: {read_dic}, readID_1: {read_dic}}
        '''
        start_multi_write = time.time()
        aux_types = {"channel_number": type("string"),
                     "median_before": type(1.0),
                     "read_number": type(10),
                     "start_mux": type(1),
                     "start_time": type(100),
                     "end_reason": type(["a", "b", "c"])}
        # check if empty dic was given
        if aux is not None:
            if len(aux) == 0:
                aux = None
        self.logger.debug("write_record_batch: Setting up batching...")
        num_reads = len(records)
        read_list = list(records.keys())
        if num_reads > batchsize:
            batches = self._get_batches(read_list, size=batchsize)
        else:
            batches = self._get_batches(read_list, size=num_reads)


        self.logger.debug("write_record_batch: batch for loop start")
        for batch in chain(batches):
            batch_len = len(batch)
            self.twrite = <slow5_rec_t **> malloc(sizeof(slow5_rec_t*)*batch_len)
            self.logger.debug("write_record_batch: _record_type_validation running")
            checked_records = {}
            checked_auxs = {}
            self.channel_number_val_array = <char **> malloc(sizeof(char*)*batch_len)
            self.median_before_val_array = <double *> malloc(sizeof(double)*batch_len)
            self.read_number_val_array = <int32_t *> malloc(sizeof(int32_t)*batch_len)
            self.start_mux_val_array = <uint8_t *> malloc(sizeof(uint8_t)*batch_len)
            self.start_time_val_array = <uint64_t *> malloc(sizeof(uint64_t)*batch_len)
            self.end_reason_val_array = <uint8_t *> malloc(sizeof(uint8_t)*batch_len)
            for i, idx in enumerate(batch):
                if aux is not None:
                    checked_record, checked_aux = self._multi_record_type_validation(records[idx], aux[idx])
                    checked_records[idx] = checked_record
                    for a in checked_aux:
                        if a == "channel_number":
                            self.channel_number_val_array[i] = strdup(checked_aux[a].encode())
                        elif a == "median_before":
                            self.median_before_val_array[i] = <double>checked_aux[a]
                        elif a == "read_number":
                            self.read_number_val_array[i] = <int32_t>checked_aux[a]
                        elif a == "start_mux":
                            self.start_mux_val_array[i] = <uint8_t>checked_aux[a]
                        elif a == "start_time":
                            self.start_time_val_array[i] = <uint64_t>checked_aux[a]
                        elif a == "end_reason":
                            self.end_reason_val_array[i] = <uint8_t>checked_aux[a]
                    checked_auxs[idx] = checked_aux
                else:
                    checked_record, checked_aux = self._record_type_validation(records[idx], aux)
                    checked_records[idx] = checked_record
            self.logger.debug("write_record_batch: _record_type_validation done")
            if len(checked_aux) == 0:
                checked_aux = None
            for idx in range(batch_len):

                # check if in append state
                # if so, we can skip header setup
                if self.state == 2:
                    self.header_state = True

                # if all checks are good, test self.header_state for 1 time write
                if not self.header_state:
                    self.logger.debug("write_record_batch: checking header stuff...")
                    error = False
                    if aux is not None:
                        if checked_aux is not None:
                            slow5_aux_types = self._aux_header_type_validation(aux_types)
                            for a in slow5_aux_types:
                                if a not in checked_aux:
                                    continue
                                elif checked_aux[a] is None:
                                    continue
                                elif slow5_aux_types[a] == SLOW5_ENUM:
                                    ret = slow5_aux_add_enum(a.encode(), <const char **>self.end_reason_labels, self.end_reason_labels_len, self.s5.header)
                                    if ret < 0:
                                        self.logger.error("write_record: slow5_aux_add_enum {}: {} could not set to C s5.header.aux_meta struct".format(a, checked_aux[a]))
                                        error = True
                                else:
                                    ret = slow5_aux_add(a.encode(), slow5_aux_types[a], self.s5.header)
                                    if ret < 0:
                                        self.logger.error("write_record_batch: slow5_aux_add {}: {} could not set to C s5.header.aux_meta struct".format(a, checked_aux[a]))
                                        error = True
                        else:
                            error = True

                    if error:
                        self.logger.error("write_record_batch: aux_meta fields failed to initialise")
                        return -1
                    # write the header
                    self.logger.debug("write_record_batch: writting header...")
                    ret = slow5_hdr_write(self.s5)
                    if ret < 0:
                        self.logger.error("write_record_batch: slow5_hdr_write could not write header")
                        return -1
                    # set state true so only done once
                    self.header_state = True
                    self.logger.debug("write_record_batch: header written")

                # Add values to read struct
                self.logger.debug("write_record_batch: slow5_rec_init()")

                self.twrite[idx] = slow5_rec_init()
                if self.twrite[idx] == NULL:
                    self.logger.error("write_record_batch: failed to allocate space for slow5 record (self.twrite[idx])")
                    return -1

                self.logger.debug("write_record_batch: self.write assignments...")
                checked_records[batch[idx]]["read_id"] = checked_records[batch[idx]]["read_id"].encode()
                self.twrite[idx].read_id = strdup(checked_records[batch[idx]]["read_id"])
                self.twrite[idx].read_id_len = len(checked_records[batch[idx]]["read_id"])
                self.twrite[idx].read_group = checked_records[batch[idx]]["read_group"]
                self.twrite[idx].digitisation = checked_records[batch[idx]]["digitisation"]
                self.twrite[idx].offset = checked_records[batch[idx]]["offset"]
                self.twrite[idx].range = checked_records[batch[idx]]["range"]
                self.twrite[idx].sampling_rate = checked_records[batch[idx]]["sampling_rate"]
                self.twrite[idx].len_raw_signal = checked_records[batch[idx]]["len_raw_signal"]
                self.logger.debug("write_record_batch: self.write.raw_signal malloc...")
                self.twrite[idx].raw_signal = <int16_t *> malloc(sizeof(int16_t)*checked_records[batch[idx]]["len_raw_signal"])
                self.logger.debug("write_record_batch: self.write.raw_signal malloc done")
                self.logger.debug("write_record_batch: self.write assignments done")

                self.logger.debug("write_record_batch: self.write processing raw_signal")
                start_write_copy_signal = time.time()
                # grabs buffer of numby array so the for loop operats in C not python = super fast
                memview = memoryview(checked_records[batch[idx]]["signal"])
                for i in range(checked_records[batch[idx]]["len_raw_signal"]):
                    self.twrite[idx].raw_signal[i] = memview[i]
                # for i in range(checked_records[batch[idx]]["len_raw_signal"]):
                #     self.twrite[idx].raw_signal[i] = checked_records[batch[idx]]["signal"][i]
                end_write_copy_signal = (time.time() - start_write_copy_signal)
                self.total_multi_write_signal_time = self.total_multi_write_signal_time + end_write_copy_signal

                self.logger.debug("write_record_batch: self.write raw_signal done")

                if aux is not None:
                    self.logger.debug("write_record_batch: aux stuff...")
                    if checked_auxs[batch[idx]] is None:
                        self.logger.error("write_record_batch: checked_aux is None".format(a, checked_auxs[batch[idx]][a]))
                        return -1

                    for a in checked_auxs[batch[idx]]:
                        self.logger.debug("write_record_batch: checked_aux: {}: {}".format(a, checked_auxs[batch[idx]][a]))
                        if checked_auxs[batch[idx]][a] is None:
                            continue
                        if a == "channel_number":
                            self.logger.debug("write_record_batch: slow5_aux_set_string running...")
                            self.logger.debug("write_record_batch: slow5_aux_set_string type: {}".format(type(checked_auxs[batch[idx]][a])))
                            ret = slow5_aux_set_string(self.twrite[idx], self.channel_number, <const char *>self.channel_number_val_array[idx], self.s5.header)
                            self.logger.debug("write_record_batch: slow5_aux_set_string running done: ret = {}".format(ret))
                            if ret < 0:
                                self.logger.error("write_record_batch: slow5_aux_set_string could not write aux value {}: {}".format(a, checked_auxs[batch[idx]][a]))
                                #### We should free here
                                return -1

                        elif a == "median_before":
                            ret = slow5_aux_set(self.twrite[idx], self.median_before, <const void *>&self.median_before_val_array[idx], self.s5.header)
                        elif a == "read_number":
                            ret = slow5_aux_set(self.twrite[idx], self.read_number, <const void *>&self.read_number_val_array[idx], self.s5.header)
                        elif a == "start_mux":
                            ret = slow5_aux_set(self.twrite[idx], self.start_mux, <const void *>&self.start_mux_val_array[idx], self.s5.header)
                        elif a == "start_time":
                            ret = slow5_aux_set(self.twrite[idx], self.start_time, <const void *>&self.start_time_val_array[idx], self.s5.header)
                        elif a == "end_reason":
                            ret = slow5_aux_set(self.twrite[idx], self.end_reason, <const void *>&self.end_reason_val_array[idx], self.s5.header)
                        else:
                            ret = -1
                        if ret < 0:
                            self.logger.error("write_record_batch: slow5_aux_set could not write aux value {}: {}".format(a, checked_aux[a]))
                            return -1

                    self.logger.debug("write_record_batch: aux stuff done")

            self.logger.debug("write_record_batch: slow5_write_batch_lazy()")

            # write the record
            if batch_len <= 0:
                self.logger.debug("write_record_batch: batch_len 0 or less")
                break

            ret = slow5_write_batch_lazy(self.twrite, self.s5, batch_len, threads)
            if ret < batch_len:
                self.logger.error("write_record_batch: write failed")
                return -1

            self.logger.debug("write_record_batch: free()")
            for i in range(batch_len):
                slow5_rec_free(self.twrite[i])
            free(self.twrite)


            self.logger.debug("write_record_batch: free() aux")
            if aux is not None:
                for i in range(batch_len):
                    free(self.channel_number_val_array[i])

                free(self.channel_number_val_array)
                free(self.median_before_val_array)
                free(self.read_number_val_array)
                free(self.start_mux_val_array)
                free(self.start_time_val_array)
                free(self.end_reason_val_array)

        end_multi_write = time.time() - start_multi_write
        self.total_multi_write_time = self.total_multi_write_time + end_multi_write
        # free memory
        # self.twrite = NULL
        self.logger.debug("write_record_batch: function complete, returning 0")
        return 0

    def close(self):
        '''
        close file so EOF is written
        '''
        if self.state in [1,2]:
            if not self.close_state:
                if self.s5 is not NULL:
                    slow5_close(self.s5)
                    self.close_state = True
                    self.logger.debug("{} closed".format(self.path))
            else:
                self.logger.warning("{} already closed".format(self.path))
        elif self.state == 0:
            if not self.close_state:
                if self.s5 is not NULL:
                    slow5_close(self.s5)
                    self.close_state = True
                    self.logger.debug("{} closed".format(self.path))
            else:
                self.logger.warning("{} already closed".format(self.path))
        else:
            self.logger.error("{} not open for writing, remake class object to refresh".format(self.path))
        return
