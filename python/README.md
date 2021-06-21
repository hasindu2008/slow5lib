# pyslow5 python library

The slow5 python library (pyslow5) allows a user to read slow5 and blow5 files.

## Installation

    git clone git@github.com:hasindu2008/slow5lib.git
    cd slow5lib
    make
    make pyslow5
    make pyslow5 install

## Usage

    import pyslow5 as slow5

    # open file
    s5 = slow5.Open('examples/example.slow5','r')

    # read all reads sequentially
    for read in s5.seq_reads(pA=True):
        print("read_id:", read['read_id'])
        print("read_group:", read['read_group'])
        print("digitisation:", read['digitisation'])
        print("offset:", read['offset'])
        print("range:", read['range'])
        print("sampling_rate:", read['sampling_rate'])
        print("len_raw_signal:", read['len_raw_signal'])
        print("signal:", read['signal'][:10])

    # read one read using readID, returns None if not found
    readID = "r4"
    read = s5.get_read(readID, pA=True)

    # random access reads from list, if read not found, returns None
    read_list = ["r1", "r3", "null_read", "r5", "r2", "r1"]
    selected_reads = s5.get_read_list(read_list)
    for r, read in zip(read_list,selected_reads):
        if read is not None:
            print(r, read['read_id'])
        else:
            print(r, "read not found")

    # Get header attributes
    attr = "flow_cell_id"
    val = s5.get_header_value(attr)
    print(f"flow_cell_id: {val}")
    attr = "exp_start_time"
    val = s5.get_header_value(attr)
    print(f"exp_start_time: {val}")
    attr = "heatsink_temp"
    val = s5.get_header_value(attr)
    print(f"heatsink_temp: {val}")
