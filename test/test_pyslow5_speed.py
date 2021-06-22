import sys
import pyslow5 as slow5
import h5py
import time
import statistics
import random


def read_multi_fast5(hdf, filename):
    '''
    read multifast5 file and return data
    '''

    for read in list(hdf.keys()):
        f5_dic = {}
        # read = "read_{}".format(readID)
        f5_dic = {'signal': [], 'readID': '', 'digitisation': 0.0,
                        'offset': 0.0, 'range': 0.0, 'sampling_rate': 0.0}

        f5_dic['signal'] = hdf[read]['Raw/Signal'][()]

        f5_dic['read_id'] = hdf[read]['Raw'].attrs['read_id'].decode()
        f5_dic['digitisation'] = hdf[read]['channel_id'].attrs['digitisation']
        f5_dic['offset'] = hdf[read]['channel_id'].attrs['offset']
        f5_dic['range'] = float("{0:.2f}".format(hdf[read]['channel_id'].attrs['range']))
        f5_dic['sampling_rate'] = hdf[read]['channel_id'].attrs['sampling_rate']

        yield f5_dic


def read_multi_fast5_single(hdf, filename, readID):
    '''
    read multifast5 file and return data
    '''
    f5_dic = {}

    # for read in list(hdf.keys()):
    read = "read_{}".format(readID)
    f5_dic = {'signal': [], 'readID': '', 'digitisation': 0.0,
                    'offset': 0.0, 'range': 0.0, 'sampling_rate': 0.0}

    f5_dic['signal'] = hdf[read]['Raw/Signal'][()]

    f5_dic['read_id'] = hdf[read]['Raw'].attrs['read_id'].decode()
    f5_dic['digitisation'] = hdf[read]['channel_id'].attrs['digitisation']
    f5_dic['offset'] = hdf[read]['channel_id'].attrs['offset']
    f5_dic['range'] = float("{0:.2f}".format(hdf[read]['channel_id'].attrs['range']))
    f5_dic['sampling_rate'] = hdf[read]['channel_id'].attrs['sampling_rate']

    return f5_dic


def read_multi_fast5_list(hdf, filename, readIDs):
    '''
    read multifast5 file and return data
    '''

    for readID in readIDs:
        f5_dic = {}
        read = "read_{}".format(readID)
        f5_dic = {'signal': [], 'readID': '', 'digitisation': 0.0,
                        'offset': 0.0, 'range': 0.0, 'sampling_rate': 0.0}

        f5_dic['signal'] = hdf[read]['Raw/Signal'][()]

        f5_dic['read_id'] = hdf[read]['Raw'].attrs['read_id'].decode()
        f5_dic['digitisation'] = hdf[read]['channel_id'].attrs['digitisation']
        f5_dic['offset'] = hdf[read]['channel_id'].attrs['offset']
        f5_dic['range'] = float("{0:.2f}".format(hdf[read]['channel_id'].attrs['range']))
        f5_dic['sampling_rate'] = hdf[read]['channel_id'].attrs['sampling_rate']

        yield f5_dic


def read_fast5(filename):
    hdf = h5py.File(filename, 'r')
    for read in read_multi_fast5(hdf, filename):
        sig = read['signal']
        readID = read['read_id']
        print(f"{readID}: {sig[:10]}")
    hdf.close()


def read_slow5(filename):
    s5 = slow5.Open(filename, 'r')
    for read in s5.seq_reads():
        sig = read['signal']
        readID = read['read_id']
        print(f"{readID}: {sig[:10]}")


def read_fast5_random_single(filename, readIDs):
    hdf = h5py.File(filename, 'r')
    for ID in readIDs:
        read = read_multi_fast5_single(hdf, filename, ID)
        sig = read['signal']
        readID = read['read_id']
        print(f"{readID}: {sig[:10]}")
    hdf.close()


def read_slow5_random_single(filename, readIDs):
    s5 = slow5.Open(filename, 'r')
    for ID in readIDs:
        read = s5.get_read(ID)
        sig = read['signal']
        readID = read['read_id']
        print(f"{readID}: {sig[:10]}")


def read_fast5_random_list(filename, readIDs):
    hdf = h5py.File(filename, 'r')
    for read in read_multi_fast5_list(hdf, filename, readIDs):
        sig = read['signal']
        readID = read['read_id']
        print(f"{readID}: {sig[:10]}")
    hdf.close()


def read_slow5_random_list(filename, readIDs):
    s5 = slow5.Open(filename, 'r')
    for read in s5.get_read_list(readIDs):
        sig = read['signal']
        readID = read['read_id']
        print(f"{readID}: {sig[:10]}")



def get_readIDs(filename):
    IDs = []
    s5 = slow5.Open(filename, 'r')
    for read in s5.seq_reads():
        IDs.append(read['read_id'])
    return IDs



# call stuff here

# files
fast5_file = sys.argv[1]
slow5_file = sys.argv[2]
blow5_file = sys.argv[3]
N = int(sys.argv[4]) # number of rounds for each test

print("|====================================|", file=sys.stderr)
print("|sequential read - 4000 reads, 1 file|", file=sys.stderr)
print("|====================================|", file=sys.stderr)
f5_times = []
s5_times = []
b5_times = []

for i in range(N):
    start_time = time.time()
    read_fast5(fast5_file)
    ttime = round(time.time() - start_time, 4)
    print(f"fast5 test {i}: {ttime} seconds", file=sys.stderr)
    f5_times.append(ttime)

for i in range(N):
    start_time = time.time()
    read_slow5(slow5_file)
    ttime = round(time.time() - start_time, 4)
    print(f"slow5 test {i}: {ttime} seconds", file=sys.stderr)
    s5_times.append(ttime)

for i in range(N):
    start_time = time.time()
    read_slow5(blow5_file)
    ttime = round(time.time() - start_time, 4)
    print(f"blow5 test {i}: {ttime} seconds", file=sys.stderr)
    b5_times.append(ttime)


print(f"fast5 mean: {statistics.mean(f5_times)}", file=sys.stderr)
print(f"fast5 stdev: {statistics.stdev(f5_times)}", file=sys.stderr)
print(f"fast5 median: {statistics.median(f5_times)}", file=sys.stderr)
print("================")
print(f"slow5 mean: {statistics.mean(s5_times)}", file=sys.stderr)
print(f"slow5 stdev: {statistics.stdev(s5_times)}", file=sys.stderr)
print(f"slow5 median: {statistics.median(s5_times)}", file=sys.stderr)
print("================")
print(f"blow5 mean: {statistics.mean(b5_times)}", file=sys.stderr)
print(f"blow5 stdev: {statistics.stdev(b5_times)}", file=sys.stderr)
print(f"blow5 median: {statistics.median(b5_times)}", file=sys.stderr)
print("==============================================================")


# ====================================================
print("|=======================================|", file=sys.stderr)
print("|random read single - 4000 reads, 1 file|", file=sys.stderr)
print("|=======================================|", file=sys.stderr)

f5_times = []
s5_times = []
b5_times = []

random.seed(1337)

readIDs = get_readIDs(slow5_file)
random.shuffle(readIDs)


for i in range(N):
    start_time = time.time()
    read_fast5_random_single(fast5_file, readIDs)
    ttime = round(time.time() - start_time, 4)
    print(f"fast5 test {i}: {ttime} seconds", file=sys.stderr)
    f5_times.append(ttime)

for i in range(N):
    start_time = time.time()
    read_slow5_random_single(slow5_file, readIDs)
    ttime = round(time.time() - start_time, 4)
    print(f"slow5 test {i}: {ttime} seconds", file=sys.stderr)
    s5_times.append(ttime)

for i in range(N):
    start_time = time.time()
    read_slow5_random_single(blow5_file, readIDs)
    ttime = round(time.time() - start_time, 4)
    print(f"blow5 test {i}: {ttime} seconds", file=sys.stderr)
    b5_times.append(ttime)


print(f"fast5 mean: {statistics.mean(f5_times)}", file=sys.stderr)
print(f"fast5 stdev: {statistics.stdev(f5_times)}", file=sys.stderr)
print(f"fast5 median: {statistics.median(f5_times)}", file=sys.stderr)
print("================")
print(f"slow5 mean: {statistics.mean(s5_times)}", file=sys.stderr)
print(f"slow5 stdev: {statistics.stdev(s5_times)}", file=sys.stderr)
print(f"slow5 median: {statistics.median(s5_times)}", file=sys.stderr)
print("================")
print(f"blow5 mean: {statistics.mean(b5_times)}", file=sys.stderr)
print(f"blow5 stdev: {statistics.stdev(b5_times)}", file=sys.stderr)
print(f"blow5 median: {statistics.median(b5_times)}", file=sys.stderr)
print("==============================================================")

# ====================================================
print("|=====================================|", file=sys.stderr)
print("|random read list - 4000 reads, 1 file|", file=sys.stderr)
print("|=====================================|", file=sys.stderr)

f5_times = []
s5_times = []
b5_times = []

random.seed(1337)

readIDs = get_readIDs(slow5_file)
random.shuffle(readIDs)


for i in range(N):
    start_time = time.time()
    read_fast5_random_list(fast5_file, readIDs)
    ttime = round(time.time() - start_time, 4)
    print(f"fast5 test {i}: {ttime} seconds", file=sys.stderr)
    f5_times.append(ttime)

for i in range(N):
    start_time = time.time()
    read_slow5_random_list(slow5_file, readIDs)
    ttime = round(time.time() - start_time, 4)
    print(f"slow5 test {i}: {ttime} seconds", file=sys.stderr)
    s5_times.append(ttime)

for i in range(N):
    start_time = time.time()
    read_slow5_random_list(blow5_file, readIDs)
    ttime = round(time.time() - start_time, 4)
    print(f"blow5 test {i}: {ttime} seconds", file=sys.stderr)
    b5_times.append(ttime)


print(f"fast5 mean: {statistics.mean(f5_times)}", file=sys.stderr)
print(f"fast5 stdev: {statistics.stdev(f5_times)}", file=sys.stderr)
print(f"fast5 median: {statistics.median(f5_times)}", file=sys.stderr)
print("================")
print(f"slow5 mean: {statistics.mean(s5_times)}", file=sys.stderr)
print(f"slow5 stdev: {statistics.stdev(s5_times)}", file=sys.stderr)
print(f"slow5 median: {statistics.median(s5_times)}", file=sys.stderr)
print("================")
print(f"blow5 mean: {statistics.mean(b5_times)}", file=sys.stderr)
print(f"blow5 stdev: {statistics.stdev(b5_times)}", file=sys.stderr)
print(f"blow5 median: {statistics.median(b5_times)}", file=sys.stderr)
print("==============================================================")
