import pyslow5
import time


print("|==============================================|")
print("|             pyslow5 test launch              |")
print("|==============================================|")


# open file and create object
start_time = time.time()
s5p = pyslow5.slow5py('examples/example.slow5','r')
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
print("get_read check, r4")
start_time = time.time()
read = s5p.get_read("r4")
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

# get yield object and iterate through reads sequentially
print("Yield check, all reads")
start_time = time.time()
reads = s5p.yield_reads()
ttime = round(time.time() - start_time, 4)
print("yield_reads in: {} seconds".format(ttime))
print("yielded readIDs:")
for read in reads:
    print(read['read_id'])

print("==============================================")

# get yield object and iterate through selected reads with random access
print("Yield check, selected reads")
read_list = ["r1", "r3", "null_read", "r5"]
start_time = time.time()
selected_reads = s5p.yield_read_list(read_list)
ttime = round(time.time() - start_time, 4)
print("yield_read_list in: {} seconds".format(ttime))
print("yielded readIDs:")
for r, read in zip(read_list,selected_reads):
    if read is not None:
        print(r, read['read_id'])
    else:
        print(r, "read not found, None returned")


print("==============================================")



print("done")
