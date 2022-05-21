//loads a batch of reads (signal+information needed for pA conversion) from file, process the batch (convert to pA and sum), and write output
//only the time for loading a batch is measured
//make zstd=1
//gcc -Wall -O2 -I include/ -o convert_to_pa test/bench/convert_to_pa.c lib/libslow5.a python/slow5threads.c -lm -lz -lzstd -lpthread  -fopenmp
//only the time for loading a batch to memory (Disk I/O + decompression + parsing and filling the memory arrays) is measured

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>
#include <omp.h>
#include <sys/time.h>
//To fetch a batch of reads I am using the inefficient fork join threading framework I wrote for the Python API
//the optimal implementation will be to use the slow5 low-level API  with a thread model in the user space
//that loads raw bytes from disk for a batch (slow5_get_next_mem())
//and then use multiple threads to decompress and parses them into the memory arrays (slow5_rec_depress_parse())
#include "../../python/slow5threads.h"

static inline double realtime(void) {
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return tp.tv_sec + tp.tv_usec * 1e-6;
}


int main(int argc, char *argv[]) {

    if(argc != 4) {
        fprintf(stderr, "Usage: %s in_file.blow5 num_thread batch_size\n", argv[0]);
        return EXIT_FAILURE;
    }

    slow5_rec_t **rec = NULL;
    int ret=1;
    int batch_size = atoi(argv[3]); // an increased batch size imrpves multi-threaded efficiecy at the cost of memory
    int num_thread = atoi(argv[2]);
    omp_set_num_threads(num_thread);

    int read_count = 0;

    double *sums = malloc(sizeof(uint64_t)*batch_size);
    double tot_time = 0;
    double t0 = 0;

    /**** Initialisation and opening of the file ***/
    t0 = realtime();

    slow5_file_t *sp = slow5_open(argv[1],"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       perror("perr: ");
       exit(EXIT_FAILURE);
    }

    tot_time += realtime() - t0;
    /**** End of init ***/

    while(ret > 0){

        /**** Fetching a batch (disk loading, decompression, parsing in to memory arrays) ***/
        t0 = realtime();
        ret = slow5_get_next_batch(&rec,sp,batch_size,num_thread);
        read_count += ret;
        tot_time += realtime() - t0;
        /**** Batch fetched ***/

        fprintf(stderr,"batch loaded with %d reads\n",ret);

        //process and print (time not measured as we want to compare to the time it takes to read the file)
        #pragma omp parallel for
        for(int i=0;i<ret;i++){
            uint64_t sum = 0;
            double scale = rec[i]->range / rec[i]->digitisation;
            for(int j=0; j<rec[i]->len_raw_signal; j++){
                sum += ((rec[i]->raw_signal[j] + rec[i]->offset) * scale);
            }
            sums[i] = sum;
        }
        fprintf(stderr,"batch processed with %d reads\n",ret);
        for(int i=0;i<ret;i++){
            fprintf(stdout,"%s\t%f\n",rec[i]->read_id,sums[i]);
        }
        fprintf(stderr,"batch printed with %d reads\n",ret);

        /**** Deinit ***/
        t0 = realtime();
        slow5_free_batch(&rec,ret);
        tot_time += realtime() - t0;
        /**** End of Deinit***/

        if(ret<batch_size){ //this indicates nothing left to read //need to handle errors
            break;
        }
    }

    /**** Deinit ***/
    t0 = realtime();
    slow5_close(sp);
    tot_time += realtime() - t0;
    /**** End of Deinit***/

    free(sums);

    fprintf(stderr,"Reads: %d\n",read_count);
    fprintf(stderr,"Time for getting samples %f\n", tot_time);

    return 0;
}
