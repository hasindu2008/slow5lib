# Miscellaneous Benchmarks

From slow5lib directory, run `test/bench/build.sh` to compile the benchmarks.

| benchmark                     | access type | threads      | description |
| -----------                   | ----------- |------------  |-------------|
|[convert_to_pa](convert_to_pa.c)                  |  sequential | openMP       | stream through the BLOW5 sequentially, while converting the signals to pA and summing up using openMP threads|
|[convert_to_pa_rand](convert_to_pa_rand.c)             |  random     | openMP       | randomly access selected readIDs, while converting the signals to pA and summing up using openMP threads|
|[get_all](get_all.c)                        |  sequential | slow_mt_lazy | stream through the BLOW5 sequentially, while sums up the signal samples. Uses inbuilt slow5_mt_lazy threads |
|[get_read_ids](get_read_ids.c)                   |  -          | -            | get the list of read IDs |
|[get_samples](get_samples.c)                    | sequential  | pthreads     | stream through the BLOW5 sequentially, sums up the signals using pthreads. I/O and processing pipelined.|
|[get_samples_no_pipelining](get_samples_no_pipelining.c)      | sequential  | pthreads     | stream through the BLOW5 sequentially, sums up the signals using pthreads. I/O and processing are not pipelined. |
|[get_read_number_rand](get_read_number_rand.c)           | random      | slow_mt_lazy | randomly access selected readIDs,  gets the read number field. Uses inbuilt slow5_mt_lazy threads. |
|[get_sample_count_rand](get_sample_count_rand.c)          | random      | slow_mt_lazy | randomly access selected readIDs,  get the len_raw_signal field. Uses inbuilt slow5_mt_lazy threads. |
|[get_samples_rand](get_samples_rand.c)               | random      | slow_mt_lazy | randomly access selected readIDs, sums up the signals. Uses inbuilt slow5_mt_lazy threads. |
