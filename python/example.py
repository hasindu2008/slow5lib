import pyslow5
import time


print("|==============================================|")
print("|             pyslow5 test launch              |")
print("|==============================================|")


# open file and create object
start_time = time.time()
s5p = pyslow5.slow5py('examples/example.slow5','r', DEBUG=1)
ttime = round(time.time() - start_time, 4)
print("slow5 file opened and object created in: {} seconds".format(ttime))
print("==============================================")

# Get data for ONE individual read, random access
print("get_read check, r1")
start_time = time.time()
read = s5p.get_read("r1")
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

print("==============================================")

# Get data for ANOTHER ONE individual read, random access, check memory
print("get_read check, r4, converte to pA")
start_time = time.time()
read = s5p.get_read("r4", pA=True)
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

print("==============================================")

# get yield object and iterate through reads sequentially
print("seq_reads check, all reads")
start_time = time.time()
print("seq_reads readIDs:")
reads = s5p.seq_reads()
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
selected_reads = s5p.get_read_list(read_list)
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
names = s5p.get_header_names()
print("header names:")
print(names)

print("==============================================")
# Get header attributes
print("Get headder attributes")
attr = "flow_cell_id"
val = s5p.get_header_value(attr)
print(f"flow_cell_id: {val}")
attr = "exp_start_time"
val = s5p.get_header_value(attr)
print(f"exp_start_time: {val}")
attr = "heatsink_temp"
val = s5p.get_header_value(attr)
print(f"heatsink_temp: {val}")
for attr in names:
    val = s5p.get_header_value(attr)
    print(f"{attr}: {val}")


print("==============================================")
print("done")
