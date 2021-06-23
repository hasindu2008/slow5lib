import pyslow5 as slow5
import time
import numpy as np


print("|==============================================|")
print("|             pyslow5 test launch              |")
print("|==============================================|")

#globals
debug = 1


# open file and create object
start_time = time.time()
s5 = slow5.Open('examples/example.slow5','r', DEBUG=debug)
ttime = round(time.time() - start_time, 4)
print(dir(s5))
print("slow5 file opened and object created in: {} seconds".format(ttime))
print("==============================================")

# # Get data for ONE individual read, random access
print("get_read check, r1")

start_time = time.time()
read = s5.get_read("r1", aux=["read_number", "start_mux", "blah"])
ttime = round(time.time() - start_time, 4)
print("get_read in: {} seconds".format(ttime))

# print all fields
print("read_id:", read['read_id'])
print("read_group:", read['read_group'])
print("digitisation:", read['digitisation'])
print("offset:", read['offset'])
print("range:", read['range'])
print("sampling_rate:", read['sampling_rate'])
print("len_raw_signal:", read['len_raw_signal'])
print("signal:", read['signal'][:10])
print("pylen of signal:", len(read['signal']))
print("AUX FIELDS:")
print("read_number:", read["read_number"])
print("start_mux:", read["start_mux"])
print("blah:", read["blah"]) #should be None


print("==============================================")

# Get data for ANOTHER ONE individual read, random access, check memory
print("get_read check, r4, converte to pA")
start_time = time.time()
read = s5.get_read("r4", pA=True)
ttime = round(time.time() - start_time, 4)
print("get_read in: {} seconds".format(ttime))

# print all fields
print("read_id:", read['read_id'])
print("read_group:", read['read_group'])
print("digitisation:", read['digitisation'])
print("offset:", read['offset'])
print("range:", read['range'])
print("sampling_rate:", read['sampling_rate'])
print("len_raw_signal:", read['len_raw_signal'])
print("signal:", read['signal'][:10])
print("pylen of signal:", len(read['signal']))
print("signal value type:", type(read['signal'][0]))
#
print("==============================================")

# get yield object and iterate through reads sequentially
print("seq_reads check, all reads")
start_time = time.time()
print("seq_reads readIDs:")
reads = s5.seq_reads()
print("type check reads:", type(reads))
for read in reads:
    print(read['read_id'])

ttime = round(time.time() - start_time, 4)
print("seq_reads in: {} seconds".format(ttime))

print("==============================================")

# get yield object and iterate through selected reads with random access
print("Yield check, selected reads")
read_list = ["r1", "r3", "null_read", "r5", "r2", "r1"]
start_time = time.time()
selected_reads = s5.get_read_list(read_list)
ttime = round(time.time() - start_time, 4)
print("get_read_list in: {} seconds".format(ttime))
print("yielded readIDs:")
for r, read in zip(read_list,selected_reads):
    if read is not None:
        print(r, read['read_id'])
    else:
        print(r, "read not found, None returned")


print("==============================================")
# get header names
print("Get headder names")
names = s5.get_header_names()
print("header names:")
print(names)
print("==============================================")

# get all headers
print("get_all_headers")
headers = s5.get_all_headers()
print(headers)


print("==============================================")
# Get header attributes
print("Get headder attributes")
attr = "flow_cell_id"
val = s5.get_header_value(attr)
print("flow_cell_id: {}".format(val))
attr = "exp_start_time"
val = s5.get_header_value(attr)
print("exp_start_time: {}".format(val))
attr = "heatsink_temp"
val = s5.get_header_value(attr)
print("heatsink_temp: {}".format(val))
for attr in names:
    val = s5.get_header_value(attr)
    print("{}: {}".format(attr, val))


s52 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
print("get_read check, 0d624d4b-671f-40b8-9798-84f2ccc4d7fc")
read1 = s52.get_read("0d624d4b-671f-40b8-9798-84f2ccc4d7fc", aux=["read_number", "start_mux", "blah"])
# print all fields
print("read_id:", read1['read_id'])
print("read_group:", read1['read_group'])
print("digitisation:", read1['digitisation'])
print("offset:", read1['offset'])
print("range:", read1['range'])
print("sampling_rate:", read1['sampling_rate'])
print("len_raw_signal:", read1['len_raw_signal'])
print("signal:", read1['signal'][:10])
print("pylen of signal:", len(read1['signal']))
print("AUX FIELDS:")
print("read_number:", read1["read_number"])
print("start_mux:", read1["start_mux"])
print("blah:", read1["blah"]) #should be None

# test for a field that doesn't exist, should return None
read2 = s52.get_read("0d624d4b-671f-40b8-9798-84f2ccc4d7fc", aux="blah")
print("AUX FIELDS:")
print("blah:", read2["blah"]) #should be None

read3 = s52.get_read("0d624d4b-671f-40b8-9798-84f2ccc4d7fc", aux="read_number")
print("AUX FIELDS:")
print("read_number:", read3["read_number"])

print("==============================================")
# get aux names
print("Get aux names")
aux_names = s52.get_aux_names()
print("aux names:")
print(aux_names)

print("==============================================")
# get aux values

print("Get aux types")
aux_types = s52.get_aux_types()
print("aux types:")
print(aux_types)
print("==============================================")

# get all aux fields
read4 = s52.get_read("0d624d4b-671f-40b8-9798-84f2ccc4d7fc", aux="all")
print("AUX FIELDS:")
for name in aux_names:
    print("{}:".format(name), read4[name])

print("==============================================")

print("seq_reads with aux:")
reads = s52.seq_reads(pA=True, aux='all')
print("type check reads:", type(reads))
for read in reads:
    print(read['read_id'])
    print("read_number", read['read_number'])

print("==============================================")
print("done")
