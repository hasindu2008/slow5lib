import pyslow5

print("test launch")

s5p = pyslow5.slow5py('examples/example.slow5','r')

# print("self.p: ", s5p.p)

# s5p = pyslow5.slow5_open_py('test/data/exp/one_fast5/exp_1_default.slow5','r')

s5p.get_read("r1")


print("done")
