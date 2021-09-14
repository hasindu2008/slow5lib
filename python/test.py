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
        self.assertEqual(sum(self.read['signal'][:10]), sum([190.26, 108.92, 109.46, 109.1, 107.67, 108.39, 108.75, 109.1, 111.07, 108.39]))


class TestAUX(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
        self.read = self.s5.get_read("r1", aux=["read_number", "start_mux", "noExistTest"])

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

    def test_seq_reads(self):
        results = ['r0', 'r1', 'r2', 'r3', 'r4', 'r5', '0a238451-b9ed-446d-a152-badd074006c4', '0d624d4b-671f-40b8-9798-84f2ccc4d7fc']
        for i, read in enumerate(self.reads):
            with self.subTest(i=i, read=read['read_id']):
                self.assertEqual(read['read_id'], results[i])


class TestYieldRead(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example.slow5','r', DEBUG=debug)

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

class testHeaders(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example.slow5','r', DEBUG=debug)

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

class testAuxAll(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
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
                self.assertEqual(sum(read['signal'][:10]), results[i][1])
                self.assertEqual(read['start_time'] ,results[i][2])




# def test_bad_type(self):
#     data = "banana"
#     with self.assertRaises(TypeError):
#         result = sum(data)

if __name__ == '__main__':
    unittest.main()
