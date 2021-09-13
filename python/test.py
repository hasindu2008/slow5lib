import unittest
import pyslow5 as slow5
import time
import numpy as np


#globals
debug = 0

class TestBase(unittest.TestCase):
    def setUp(self):
        self.s5 = slow5.Open('examples/example.slow5','r', DEBUG=debug)
        self.read = self.s5.get_read("r1")

    def test_class(self):
        """
        Test pyslow5 class methods
        First set up the class with sample data
        """
        result = dir(self.s5)
        self.assertEqual(result, ['__class__', '__delattr__', '__dir__', '__doc__', '__eq__', '__format__', '__ge__', '__getattribute__', '__gt__', '__hash__', '__init__', '__init_subclass__', '__le__', '__lt__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__setstate__', '__sizeof__', '__str__', '__subclasshook__', '_convert_to_pA', '_get_read', '_get_read_aux', '_get_seq_read_aux', 'get_all_headers', 'get_aux_names', 'get_aux_types', 'get_header_names', 'get_header_value', 'get_read', 'get_read_list', 'seq_reads'])

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



    # def test_bad_type(self):
    #     data = "banana"
    #     with self.assertRaises(TypeError):
    #         result = sum(data)



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
        debug = 0
        self.s5 = slow5.Open('examples/example.slow5','r', DEBUG=debug)
        self.read = self.s5.get_read("r1")



if __name__ == '__main__':
    unittest.main()
