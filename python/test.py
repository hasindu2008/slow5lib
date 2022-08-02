import unittest
import pyslow5 as slow5
import time
import numpy as np

"""
    Run from root dir of repo after making pyslow5

    python3 -m unittest -v python/test.py

"""

#globals
debug = 0 #TODO: make this an argument with -v

class TestBase(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example.slow5','r', DEBUG=debug)
        self.read = self.s5.get_read("r1")
    def tearDown(self):
        self.s5.close()
    def test_class_methods(self):
        result = type(self.s5)
        self.assertEqual(str(result), "<class 'pyslow5.Open'>")
    def test_read_type(self):
        self.assertEqual(str(type(self.read)), "<class 'dict'>")
    def test_read_id(self):
        self.assertEqual(self.read['read_id'], "r1")
    def test_read_group(self):
        self.assertEqual(self.read['read_group'], 0)
    def test_digitisation(self):
        self.assertEqual(self.read['digitisation'], 8192.0)
    def test_offset(self):
        self.assertEqual(self.read['offset'], 23.0)
    def test_range(self):
        self.assertEqual(self.read['range'], 1467.61)
    def test_sampling_rate(self):
        self.assertEqual(self.read['sampling_rate'], 4000.0)
    def test_len_raw_signal(self):
        self.assertEqual(self.read['len_raw_signal'], 59676)
    def test_pylen_signal(self):
        self.assertEqual(len(self.read['signal']), 59676)
    def test_signal(self):
        self.assertEqual(sum(self.read['signal'][:10]), sum([1039,588,588,593,586,574,570,585,588,586]))


class TestRandomAccess(unittest.TestCase):
    """
    Get data for ANOTHER ONE individual read, random access, check memory
    """
    def setUp(self):
        self.s5 = slow5.Open('examples/example.slow5','r', DEBUG=debug)
        self.read = self.s5.get_read("r1")
        self.read = self.s5.get_read("r4", pA=True)
    def tearDown(self):
        self.s5.close()
    def test_read_id(self):
        self.assertEqual(self.read['read_id'], "r4")
    def test_read_group(self):
        self.assertEqual(self.read['read_group'], 0)
    def test_digitisation(self):
        self.assertEqual(self.read['digitisation'], 8192.0)
    def test_offset(self):
        self.assertEqual(self.read['offset'], 23.0)
    def test_range(self):
        self.assertEqual(self.read['range'], 1467.61)
    def test_sampling_rate(self):
        self.assertEqual(self.read['sampling_rate'], 4000.0)
    def test_len_raw_signal(self):
        self.assertEqual(self.read['len_raw_signal'], 59670)
    def test_pylen_signal(self):
        self.assertEqual(len(self.read['signal']), 59670)
    def test_signal(self):
        self.assertEqual(int(sum([round(i, 2) for i in self.read['signal'][:10]])), int(sum([190.26, 108.92, 109.46, 109.1, 107.67, 108.39, 108.75, 109.1, 111.07, 108.39])))


class TestAUX(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
        self.read = self.s5.get_read("r1", aux=["read_number", "start_mux", "noExistTest"])
    def tearDown(self):
        self.s5.close()
    def test_read_id(self):
        self.assertEqual(self.read['read_id'], "r1")
    def test_read_number(self):
        self.assertEqual(self.read['read_number'], 2287)
    def test_start_mux(self):
        self.assertEqual(self.read['start_mux'], 2)
    def test_nonExistant_aux(self):
        self.assertIs(self.read['noExistTest'], None)

class TestSequentialRead(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
        self.reads = self.s5.seq_reads()
    def tearDown(self):
        self.s5.close()
    def test_seq_reads(self):
        results = ['r0', 'r1', 'r2', 'r3', 'r4', 'r5', '0a238451-b9ed-446d-a152-badd074006c4', '0d624d4b-671f-40b8-9798-84f2ccc4d7fc']
        for i, read in enumerate(self.reads):
            with self.subTest(i=i, read=read['read_id']):
                self.assertEqual(read['read_id'], results[i])


class TestYieldRead(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example.slow5','r', DEBUG=debug)
    def tearDown(self):
        self.s5.close()
    def test_base_reads(self):
        read_list = ["r1", "r3", "r5", "r2", "r1"]
        results = ["r1", "r3", "r5", "r2", "r1"]
        selected_reads = self.s5.get_read_list(read_list)
        for i, read in enumerate(selected_reads):
            if read is None:
                sate = None
            else:
                state = read['read_id']
            with self.subTest(i=i, read=state):
                if read is None:
                    self.assertEqual(read, results[i])
                else:
                    self.assertEqual(read['read_id'], results[i])
    def test_reads_with_no_exist(self):
        read_list = ["r1", "r3", "null_read", "r5", "r2", "r1"]
        results = ["r1", "r3", None, "r5", "r2", "r1"]
        selected_reads = self.s5.get_read_list(read_list)
        for i, read in enumerate(selected_reads):
            if read is None:
                sate = None
            else:
                state = read['read_id']
            with self.subTest(i=i, read=state):
                if read is None:
                    self.assertEqual(read, results[i])
                else:
                    self.assertEqual(read['read_id'], results[i])

class TestHeaders(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example.slow5','r', DEBUG=debug)
    def tearDown(self):
        self.s5.close()
    def test_get_header_names(self):
        results = ['asic_id', 'asic_id_eeprom', 'asic_temp', 'auto_update', 'auto_update_source',
                   'bream_core_version', 'bream_is_standard', 'bream_map_version', 'bream_ont_version',
                   'bream_prod_version', 'bream_rnd_version', 'device_id', 'exp_script_name', 'exp_script_purpose',
                   'exp_start_time', 'experiment_kit', 'experiment_type', 'file_version', 'filename', 'flow_cell_id',
                   'heatsink_temp', 'hostname', 'installation_type', 'local_firmware_file', 'operating_system',
                   'protocol_run_id', 'protocols_version', 'run_id', 'sample_frequency', 'usb_config',
                   'user_filename_input', 'version']
        names = self.s5.get_header_names()
        self.assertEqual(names, results)
    def test_get_all_headers(self):
        results = ['asic_id', 'asic_id_eeprom', 'asic_temp', 'auto_update', 'auto_update_source',
                   'bream_core_version', 'bream_is_standard', 'bream_map_version', 'bream_ont_version',
                   'bream_prod_version', 'bream_rnd_version', 'device_id', 'exp_script_name', 'exp_script_purpose',
                   'exp_start_time', 'experiment_kit', 'experiment_type', 'file_version', 'filename', 'flow_cell_id',
                   'heatsink_temp', 'hostname', 'installation_type', 'local_firmware_file', 'operating_system',
                   'protocol_run_id', 'protocols_version', 'run_id', 'sample_frequency', 'usb_config',
                   'user_filename_input', 'version']
        headers = self.s5.get_all_headers()
        self.assertEqual(list(headers.keys()), results)
    def test_get_all_headers(self):
        attr = "flow_cell_id"
        result = ""
        val = self.s5.get_header_value(attr)
        self.assertEqual(val, result)
    def test_get_all_headers(self):
        attr = "exp_start_time"
        result = ""
        val = self.s5.get_header_value(attr)
        self.assertEqual(val, result)
    def test_get_all_headers(self):
        attr = "heatsink_temp"
        result = ""
        val = self.s5.get_header_value(attr)
        self.assertEqual(val, result)
    def test_get_all_headers(self):
        results = ["3574887596", "0", "29.2145729", "1", "https://mirror.oxfordnanoportal.com/software/MinKNOW/", "1.1.20.1",
                   "1", "1.1.20.1", "1.1.20.1", "1.1.20.1", "0.1.1", "MN16450", "python/recipes/nc/NC_48Hr_Sequencing_Run_FLO-MIN106_SQK-LSK108.py",
                   "sequencing_run", "1479433093", "genomic_dna", "customer_qc", "1", "deamernanopore_20161117_fnfab43577_mn16450_sequencing_run_ma_821_r9_4_na12878_11_17_16_88738",
                   "FAB43577", "33.9921875", "DEAMERNANOPORE", "map", "0", "Windows 6.2", "a4429838-103c-497f-a824-7dffa72cfd81",
                   "1.1.20", "d6e473a6d513ec6bfc150c60fd4556d72f0e6d18", "4000", "1.0.11_ONT#MinION_fpga_1.0.1#ctrl#Auto",
                   "ma_821_r9.4_na12878_11_17_16", "1.1.20"]
        names = self.s5.get_header_names()
        for i, attr in enumerate(names):
            with self.subTest(i=i, attr=attr):
                val = self.s5.get_header_value(attr)
                self.assertEqual(val, results[i])

class TestAuxAll(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
    def tearDown(self):
        self.s5.close()
    def test_get_aux_names(self):
        result = ['channel_number', 'median_before', 'read_number', 'start_mux', 'start_time']
        aux_names = self.s5.get_aux_names()
        self.assertEqual(aux_names, result)
    def test_get_aux_types(self):
        result = [22, 9, 2, 4, 7]
        aux_types = self.s5.get_aux_types()
        aux_names = self.s5.get_aux_names()
        self.assertEqual(aux_types, result)
    def test_get_read_all_aux(self):
        results = ['391', 260.557264, 2287, 2, 36886851]
        read = self.s5.get_read("0d624d4b-671f-40b8-9798-84f2ccc4d7fc", aux="all")
        aux_names = self.s5.get_aux_names()
        for i, name in enumerate(aux_names):
            with self.subTest(i=i, attr=name):
                self.assertEqual(read[name], results[i])
    def test_seq_reads_pA_aux_all(self):
        results = [['r0', 1106.3899999999999, 78470500],
                   ['r1', 1585.4299999999998, 36886851],
                   ['r2', 1106.3899999999999, 78470500],
                   ['r3', 1585.4299999999998, 36886851],
                   ['r4', 1106.3899999999999, 78470500],
                   ['r5', 1585.4299999999998, 36886851],
                   ['0a238451-b9ed-446d-a152-badd074006c4', 1106.3899999999999, 78470500],
                   ['0d624d4b-671f-40b8-9798-84f2ccc4d7fc', 1585.4299999999998, 36886851]]
        reads = self.s5.seq_reads(pA=True, aux='all')
        for i, read in enumerate(reads):
            with self.subTest(i=i, attr=read['read_id']):
                self.assertEqual(read['read_id'], results[i][0])
                self.assertEqual(int(sum([round(i, 2) for i in read['signal'][:10]])), int(results[i][1]))
                self.assertEqual(read['start_time'] ,results[i][2])

class TestWrite(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
        self.F = slow5.Open('examples/example_write.slow5','w', DEBUG=debug)
    def tearDown(self):
        self.s5.close()
        self.F.close()
    def test_get_empty_header(self):
        results = {"asic_id": None,
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
        header = self.F.get_empty_header()
        for i, head in enumerate(header):
            with self.subTest(i=i, attr=head):
                self.assertEqual(header[head], results[head])
    def test_write_header(self):
        # This doesn't actually write the header, but adds it to object
        # Header is actually written when writing first record
        results = 0
        header = self.F.get_empty_header()
        counter = 0
        for i in header:
            header[i] = "test_{}".format(counter)
            counter += 1
        ret = self.F.write_header(header)
        self.assertEqual(ret, results)
    def test_get_empty_record(self):
        results = {"read_id": None,
                  "read_group": 0,
                  "digitisation": None,
                  "offset": None,
                  "range": None,
                  "sampling_rate": None,
                  "len_raw_signal": None,
                  "signal": None}
        record = self.F.get_empty_record()
        for i, rec in enumerate(record):
            with self.subTest(i=i, attr=rec):
                self.assertEqual(record[rec], results[rec])
    def test_write_records(self):
        results = [0]*8
        ret_list = []
        header = self.F.get_empty_header()
        counter = 0
        for i in header:
            header[i] = "test_{}".format(counter)
            counter += 1
        ret = self.F.write_header(header)
        reads = self.s5.seq_reads()
        for read in reads:
            record = self.F.get_empty_record()
            for i in read:
                if i in record:
                    record[i] = read[i]
            ret = self.F.write_record(record)
            ret_list.append(ret)
        self.assertEqual(ret_list, results)


class testMultiThreading(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
    def tearDown(self):
        self.s5.close()
    def test_seq_reads_multi(self):
        results = [['r0', 1106.3899999999999, 78470500],
                   ['r1', 1585.4299999999998, 36886851],
                   ['r2', 1106.3899999999999, 78470500],
                   ['r3', 1585.4299999999998, 36886851],
                   ['r4', 1106.3899999999999, 78470500],
                   ['r5', 1585.4299999999998, 36886851],
                   ['0a238451-b9ed-446d-a152-badd074006c4', 1106.3899999999999, 78470500],
                   ['0d624d4b-671f-40b8-9798-84f2ccc4d7fc', 1585.4299999999998, 36886851]]
        reads = self.s5.seq_reads_multi(threads=2, batchsize=3, pA=True, aux='all')
        for i, read in enumerate(reads):
            with self.subTest(i=i, attr=read['read_id']):
                print(read['read_id'])
                self.assertEqual(read['read_id'], results[i][0])
                self.assertEqual(int(sum([round(i, 2) for i in read['signal'][:10]])), int(results[i][1]))
                self.assertEqual(read['start_time'] ,results[i][2])


class testMultiThreadingRandom(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
    def tearDown(self):
        self.s5.close()
    def test_get_reads_multi(self):
        results = [['r0', 1106.3899999999999, 78470500],
                   ['r1', 1585.4299999999998, 36886851],
                   ['r2', 1106.3899999999999, 78470500],
                   ['r3', 1585.4299999999998, 36886851],
                   ['r4', 1106.3899999999999, 78470500],
                   ['r5', 1585.4299999999998, 36886851],
                   ['0a238451-b9ed-446d-a152-badd074006c4', 1106.3899999999999, 78470500],
                   ['0d624d4b-671f-40b8-9798-84f2ccc4d7fc', 1585.4299999999998, 36886851]]
        read_list = [i[0] for i in results]
        reads = self.s5.get_read_list_multi(read_list, threads=2, batchsize=3, pA=True, aux='all')
        for i, read in enumerate(reads):
            with self.subTest(i=i, attr=read['read_id']):
                print(read['read_id'])
                self.assertEqual(read['read_id'], results[i][0])
                self.assertEqual(int(sum([round(i, 2) for i in read['signal'][:10]])), int(results[i][1]))
                self.assertEqual(read['start_time'] ,results[i][2])


class TestWriteData(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
        self.F = slow5.Open('examples/example_write.slow5','r', DEBUG=debug)
    def tearDown(self):
        self.s5.close()
        self.F.close()
    def test_write_matches_read(self):
        reads = self.s5.seq_reads()
        for read in reads:
            wread = self.F.get_read(read["read_id"])
            for i, rec in enumerate(wread):
                # TODO: remove this when read_groups are added
                with self.subTest(i=i, attr=rec):
                    if rec == "read_group":
                        continue
                    if rec == "signal":
                        self.assertEqual(int(sum([round(i, 2) for i in wread[rec][:10]])), int(sum([round(i, 2) for i in read[rec][:10]])))
                    else:
                        self.assertEqual(wread[rec], read[rec])


class TestWriteAux(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
        self.F = slow5.Open('examples/example_write_aux.slow5','w', DEBUG=debug)
    def tearDown(self):
        self.s5.close()
        self.F.close()
    def test_get_empty_aux(self):
        results = {"channel_number": None,
               "median_before": None,
               "read_number": None,
               "start_mux": None,
               "start_time": None,
               "end_reason": None}
        _, aux = self.F.get_empty_record(aux=True)
        for i, rec in enumerate(aux):
            with self.subTest(i=i, attr=rec):
                self.assertEqual(aux[rec], results[rec])
    def test_write_records_aux(self):
        results = [0]*8
        ret_list = []
        header = self.F.get_empty_header()
        counter = 0
        for i in header:
            header[i] = "test_{}".format(counter)
            counter += 1
        ret = self.F.write_header(header)
        reads = self.s5.seq_reads(aux='all')
        for read in reads:
            record, aux = self.F.get_empty_record(aux=True)
            for i in read:
                if i in record:
                    record[i] = read[i]
                if i in aux:
                    aux[i] = read[i]
            ret = self.F.write_record(record, aux)
            ret_list.append(ret)
        self.assertEqual(ret_list, results)


# def test_bad_type(self):
#     data = "banana"
#     with self.assertRaises(TypeError):
#         result = sum(data)

if __name__ == '__main__':
    unittest.main()
