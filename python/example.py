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

s5.close()
print("==============================================")
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

s52.close()
print("==============================================")

print("write reads no aux")

F = slow5.Open('examples/example_write.slow5','w', DEBUG=debug)
header = F.get_empty_header()

counter = 0
for i in header:
    header[i] = "test_{}".format(counter)
    counter += 1

ret = F.write_header(header)
print("ret: write_header(): {}".format(ret))

s58 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
reads = s58.seq_reads()

for read in reads:
    record = F.get_empty_record()
    for i in read:
        if i in record:
            record[i] = read[i]
    ret = F.write_record(record)
    print("ret: write_record(): {}".format(ret))

F.close()
s58.close()

print("==============================================")

print("append reads no aux")

F = slow5.Open('examples/example_write_append.blow5','w', rec_press="none", sig_press="none", DEBUG=debug)
header = F.get_empty_header()

counter = 0
for i in header:
    header[i] = "test_{}".format(counter)
    counter += 1

ret = F.write_header(header)
print("ret: write_header(): {}".format(ret))

s58 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
reads = s58.seq_reads()

read_count = 0
for read in reads:
    record = F.get_empty_record()
    for i in read:
        if i in record:
            record[i] = read[i]
    ret = F.write_record(record)
    print("ret: write_record(): {}".format(ret))

s58.close()
F.close()

F2 = slow5.Open('examples/example_write_append.blow5','a', DEBUG=debug)

print("get_all_headers")
headers = F2.get_all_headers()
print(headers)

s58 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
reads = s58.seq_reads()

for read in reads:
    record = F2.get_empty_record()
    for i in read:
        if i in record:
            if i == "read_id":
                j = read[i]+"_append"
                record[i] = j
            else:
                record[i] = read[i]
    ret = F2.write_record(record)
    print("ret: write_record(): {}".format(ret))

s58.close()
F2.close()


print("==============================================")
print("write reads with aux")

F = slow5.Open('examples/example_write_aux.blow5','w', DEBUG=debug)
header = F.get_empty_header()
header2 = F.get_empty_header()

counter = 0
for i in header:
    header[i] = "test_{}".format(counter)
    counter += 1

for i in header2:
    header2[i] = "test_{}".format(counter)
    counter += 1

ret = F.write_header(header)
print("ret: write_header(): {}".format(ret))
ret = F.write_header(header2, read_group=1)
print("ret: write_header(): {}".format(ret))

s58 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
reads = s58.seq_reads(aux='all')

for read in reads:
    record, aux = F.get_empty_record(aux=True)
    for i in read:
        if i in record:
            record[i] = read[i]
        if i in aux:
            aux[i] = read[i]
    ret = F.write_record(record, aux)
    print("ret: write_record(): {}".format(ret))

s58.close()
F.close()

print("==============================================")
print("append reads with aux")

F = slow5.Open('examples/example_write_append_aux.blow5','w', rec_press="none", sig_press="none", DEBUG=debug)
header = F.get_empty_header()
header2 = F.get_empty_header()

counter = 0
for i in header:
    header[i] = "test_{}".format(counter)
    counter += 1

for i in header2:
    header2[i] = "test_{}".format(counter)
    counter += 1

ret = F.write_header(header)
print("ret: write_header(): {}".format(ret))
ret = F.write_header(header2, read_group=1)
print("ret: write_header(): {}".format(ret))

s58 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
reads = s58.seq_reads(aux='all')

for read in reads:
    record, aux = F.get_empty_record(aux=True)
    for i in read:
        if i in record:
            record[i] = read[i]
        if i in aux:
            aux[i] = read[i]
    ret = F.write_record(record, aux)
    print("ret: write_record(): {}".format(ret))

s58.close()
F.close()

F2 = slow5.Open('examples/example_write_append_aux.blow5','a', DEBUG=debug)

s58 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
reads = s58.seq_reads(aux='all')

for read in reads:
    record, aux = F2.get_empty_record(aux=True)
    for i in read:
        if i in record:
            if i == "read_id":
                j = read[i]+"_append"
                record[i] = j
            else:
                record[i] = read[i]
        if i in aux:
            aux[i] = read[i]
    ret = F2.write_record(record, aux)
    print("ret: write_record(): {}".format(ret))

s58.close()
F2.close()



print("==============================================")

print("seq_reads_multi with aux:")
s53 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
reads = s53.seq_reads_multi(threads=2, batchsize=3, pA=True, aux='all')
print("type check reads:", type(reads))
for read in reads:
    print(read['read_id'])
    print("read_number", read['read_number'])

print("==============================================")

print("get_reads_multi with aux:")

read_list = ['r0',
             'r1',
             'r2',
             'r3',
             'r4',
             'r5',
             '0a238451-b9ed-446d-a152-badd074006c4',
             '0d624d4b-671f-40b8-9798-84f2ccc4d7fc']

s53.close()

s53 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
reads = s53.get_read_list_multi(read_list, threads=2, batchsize=3, pA=True, aux='all')
print("type check reads:", type(reads))
for read in reads:
    print(read['read_id'])
    print("read_number", read['read_number'])

s53.close()
print("==============================================")
print("write reads with aux multi")

F = slow5.Open('examples/example_write_aux_multi.slow5','w', DEBUG=debug)
header = F.get_empty_header()
header2 = F.get_empty_header()

counter = 0
for i in header:
    header[i] = "test_{}".format(counter)
    counter += 1

for i in header2:
    header2[i] = "test_{}".format(counter)
    counter += 1

ret = F.write_header(header)
print("ret: write_header(): {}".format(ret))
ret = F.write_header(header2, read_group=1)
print("ret: write_header(): {}".format(ret))

s58 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)
reads = s58.seq_reads(aux='all')

records = {}
auxs = {}
for read in reads:
    record, aux = F.get_empty_record(aux=True)
    # record = F.get_empty_record()
    for i in read:
        if i == "read_id":
            readID = read[i]
        if i in record:
            record[i] = read[i]
        if i in aux:
            aux[i] = read[i]
    records[readID] = record
    auxs[readID] = aux
print(records)
print(auxs)
ret = F.write_record_batch(records, threads=2, batchsize=3, aux=auxs)
print("ret: write_record(): {}".format(ret))

s58.close()
F.close()

print("==============================================")
print("get all readIDs from index")

s58 = slow5.Open('examples/example2.slow5','r', DEBUG=debug)

read_ids, num_reads = s58.get_read_ids()

print(read_ids)
print("number of reads: {}".format(num_reads))

s58.close()

print("==============================================")
print("get enum fields")

s59 = slow5.Open('examples/adv/example3.blow5','r', DEBUG=debug)

e = s59.get_aux_enum_labels('end_reason')

print(e)

s59.close()

print("==============================================")
print("check can read enum aux field")

s510 = slow5.Open('examples/adv/example3.blow5','r', DEBUG=debug)
reads = s510.seq_reads(aux='all')

e = s510.get_aux_enum_labels('end_reason')

for read in reads:
    print(read['read_id'], read['end_reason'], e[read['end_reason']])

s510.close()

print("==============================================")
print("write reads with aux and end_reason enum")

F = slow5.Open('examples/example_write_aux_enum.blow5','w', DEBUG=debug)
header, end_reason_labels = F.get_empty_header(aux=True)

counter = 0
for i in header:
    header[i] = "test_{}".format(counter)
    counter += 1

ret = F.write_header(header, end_reason_labels=end_reason_labels)
print("ret: write_header(): {}".format(ret))

s58 = slow5.Open('examples/adv/example3.blow5','r', DEBUG=debug)
reads = s58.seq_reads(aux='all')

for read in reads:
    record, aux = F.get_empty_record(aux=True)
    for i in read:
        if i in record:
            record[i] = read[i]
        if i in aux:
            aux[i] = read[i]
    ret = F.write_record(record, aux)
    print("ret: write_record(): {}".format(ret))

s58.close()
F.close()

print("==============================================")
print("write reads with aux multi")

F = slow5.Open('examples/example_write_aux_enum_multi.blow5','w', DEBUG=debug)
header, end_reason_labels = F.get_empty_header(aux=True)
header2 = F.get_empty_header()

counter = 0
for i in header:
    header[i] = "test_{}".format(counter)
    counter += 1

for i in header2:
    header2[i] = "test_{}".format(counter)
    counter += 1

ret = F.write_header(header, end_reason_labels=end_reason_labels)
print("ret: write_header(): {}".format(ret))
ret = F.write_header(header2, read_group=1)
print("ret: write_header(): {}".format(ret))

s58 = slow5.Open('examples/adv/example3.blow5','r', DEBUG=debug)
reads = s58.seq_reads(aux='all')

records = {}
auxs = {}
for read in reads:
    record, aux = F.get_empty_record(aux=True)
    # record = F.get_empty_record()
    for i in read:
        if i == "read_id":
            readID = read[i]
        if i in record:
            record[i] = read[i]
        if i in aux:
            aux[i] = read[i]
    records[readID] = record
    auxs[readID] = aux
print(records)
print(auxs)
ret = F.write_record_batch(records, threads=2, batchsize=3, aux=auxs)
print("ret: write_record(): {}".format(ret))

s58.close()
F.close()

print("==============================================")
# print("seq_reads with big file:")
# start_time = time.time()
# s53 = slow5.Open('/home/jamfer/Data/SK/multi_fast5/s5/FAK40634_d1cc054609fe2c5fcdeac358864f9dc81c8bb793_95.blow5','r', DEBUG=debug)
# reads = s53.seq_reads(pA=True, aux='all')
# # print("type check reads:", type(reads))
# for read in reads:
#     print(read['read_id'])
#     # print("read_number", read['read_number'])
# seq_time = round(time.time() - start_time, 4)
# print("get_read in: {} seconds".format(seq_time))
#
# print("==============================================")
# print("seq_reads_multi with big file:")
# start_time = time.time()
# s53 = slow5.Open('/home/jamfer/Data/SK/multi_fast5/s5/FAK40634_d1cc054609fe2c5fcdeac358864f9dc81c8bb793_95.blow5','r', DEBUG=debug)
# reads = s53.seq_reads_multi(threads=8, batchsize=10, pA=True, aux='all')
# # print("type check reads:", type(reads))
# for read in reads:
#     print(read['read_id'])
#     # print("read_number", read['read_number'])
# seq_multi_time = round(time.time() - start_time, 4)
# print("get_read in: {} seconds".format(seq_multi_time))
#
# print("==============================================")
#
# print("single seq/write with big file:")
# start_time = time.time()
# sR = slow5.Open('/home/jamfer/Data/SK/multi_fast5/s5/FAK40634_d1cc054609fe2c5fcdeac358864f9dc81c8bb793_95.blow5','r', DEBUG=debug)
# reads = sR.seq_reads(aux='all')
# sW = slow5.Open('/home/jamfer/Data/SK/multi_fast5/s5/single_read_write.blow5','w', DEBUG=debug)
#
# header = F.get_empty_header()
#
# counter = 0
# for i in header:
#     header[i] = "test_{}".format(counter)
#     counter += 1
#
# ret = sW.write_header(header)
# print("ret: write_header(): {}".format(ret))
#
# for read in reads:
#     record, aux = sW.get_empty_record(aux=True)
#     for i in read:
#         if i in record:
#             record[i] = read[i]
#         if i in aux:
#             aux[i] = read[i]
#     ret = sW.write_record(record, aux)
#
# single_seq_write_time = round(time.time() - start_time, 4)
# print("single seq/write in: {} seconds".format(single_seq_write_time))
#
# print("==============================================")
# print("multi seq, single write with big file:")
# start_time = time.time()
# sR = slow5.Open('/home/jamfer/Data/SK/multi_fast5/s5/FAK40634_d1cc054609fe2c5fcdeac358864f9dc81c8bb793_95.blow5','r', DEBUG=debug)
# reads = sR.seq_reads_multi(threads=4, batchsize=100, aux='all')
# sW = slow5.Open('/home/jamfer/Data/SK/multi_fast5/s5/multi_read_single_write.blow5','w', DEBUG=debug)
# header = F.get_empty_header()
#
# counter = 0
# for i in header:
#     header[i] = "test_{}".format(counter)
#     counter += 1
#
# ret = sW.write_header(header)
# print("ret: write_header(): {}".format(ret))
#
# for read in reads:
#     record, aux = sW.get_empty_record(aux=True)
#     for i in read:
#         if i in record:
#             record[i] = read[i]
#         if i in aux:
#             aux[i] = read[i]
#     ret = sW.write_record(record, aux)
#
# multi_seq_single_write_time = round(time.time() - start_time, 4)
# print("multi seq, single write in: {} seconds".format(multi_seq_single_write_time))
#
# print("==============================================")
# print("single seq, multi write with big file:")
# start_time = time.time()
# sR = slow5.Open('/home/jamfer/Data/SK/multi_fast5/s5/FAK40634_d1cc054609fe2c5fcdeac358864f9dc81c8bb793_95.blow5','r', DEBUG=debug)
# reads = sR.seq_reads(aux='all')
# sW = slow5.Open('/home/jamfer/Data/SK/multi_fast5/s5/single_read_multi_write.blow5','w', DEBUG=debug)
#
# header = sW.get_empty_header()
#
# counter = 0
# for i in header:
#     header[i] = "test_{}".format(counter)
#     counter += 1
#
# ret = sW.write_header(header)
# print("ret: write_header(): {}".format(ret))
#
# records = {}
# auxs = {}
# for read in reads:
#     record, aux = sW.get_empty_record(aux=True)
#     for i in read:
#         if i == "read_id":
#             readID = read[i]
#         if i in record:
#             record[i] = read[i]
#         if i in aux:
#             aux[i] = read[i]
#     records[readID] = record
#     auxs[readID] = aux
#     if len(records) >= 100:
#         ret = sW.write_record_batch(records, threads=4, batchsize=100, aux=auxs)
#         records = {}
#         auxs = {}
#
# single_seq_multi_write_time = round(time.time() - start_time, 4)
# print("single seq, multi write in: {} seconds".format(single_seq_multi_write_time))
#
# print("==============================================")
# print("multi seq/write with big file:")
# start_time = time.time()
# sR = slow5.Open('/home/jamfer/Data/SK/multi_fast5/s5/FAK40634_d1cc054609fe2c5fcdeac358864f9dc81c8bb793_95.blow5','r', DEBUG=debug)
# reads = sR.seq_reads_multi(threads=4, batchsize=100, aux='all')
# sW = slow5.Open('/home/jamfer/Data/SK/multi_fast5/s5/multi_read_write.blow5','w', DEBUG=debug)
#
#
# header = sW.get_empty_header()
#
# counter = 0
# for i in header:
#     header[i] = "test_{}".format(counter)
#     counter += 1
#
# ret = sW.write_header(header)
# print("ret: write_header(): {}".format(ret))
#
# records = {}
# auxs = {}
# for read in reads:
#     record, aux = sW.get_empty_record(aux=True)
#     for i in read:
#         if i == "read_id":
#             readID = read[i]
#         if i in record:
#             record[i] = read[i]
#         if i in aux:
#             aux[i] = read[i]
#     records[readID] = record
#     auxs[readID] = aux
#     if len(records) >= 100:
#         ret = sW.write_record_batch(records, threads=4, batchsize=100, aux=auxs)
#         records = {}
#         auxs = {}
# multi_seq_write_time = round(time.time() - start_time, 4)
# print("multi seq/write in: {} seconds".format(multi_seq_write_time))
#
# print("==============================================")
#
# print("big file times:")
# print("single seq/write in: {} seconds".format(single_seq_write_time))
# print("multi seq, single write in: {} seconds".format(multi_seq_single_write_time))
# print("single seq, multi write in: {} seconds".format(single_seq_multi_write_time))
# print("multi seq/write in: {} seconds".format(multi_seq_write_time))

print("done")
