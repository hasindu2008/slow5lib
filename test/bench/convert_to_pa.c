//get all the samples and sum them to stdout
//make zstd=1
//gcc -Wall -O2 -I include/ -o convert_to_pa test/bench/convert_to_pa.c lib/libslow5.a python/slow5threads.c -lm -lz -lzstd -lpthread  -fopenmp

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>
#include <omp.h>
#include <sys/time.h>
#include "../../python/slow5threads.h"

static inline double realtime(void) {
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return tp.tv_sec + tp.tv_usec * 1e-6;
}


int main(int argc, char *argv[]) {

    if(argc != 4) {
        fprintf(stderr, "Usage: %s in_file.blow5  num_thread batch_size\n", argv[0]);
        return EXIT_FAILURE;
    }

    slow5_rec_t **rec = NULL;
    int ret=1;
    int batch_size = atoi(argv[3]);
    int num_thread = atoi(argv[2]);
    omp_set_num_threads(num_thread);

    int read_count = 0;

    double *sums = malloc(sizeof(uint64_t)*batch_size);

    double tot_time = 0;
    double t0 = realtime();

    slow5_file_t *sp = slow5_open(argv[1],"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       perror("perr: ");
       exit(EXIT_FAILURE);
    }

    tot_time += realtime() - t0;

    while(ret > 0){

        t0 = realtime();
        ret = slow5_get_next_batch(&rec,sp,batch_size,num_thread);
        read_count += ret;
        tot_time += realtime() - t0;
        fprintf(stderr,"batch loaded with %d reads\n",ret);

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

        t0 = realtime();
        slow5_free_batch(&rec,ret);
        tot_time += realtime() - t0;

        if(ret<batch_size){ //this indicates nothing left to read //need to handle errors
            break;
        }
    }

    t0 = realtime();
    slow5_close(sp);
    tot_time += realtime() - t0;

    free(sums);

    fprintf(stderr,"Reads: %d\n",read_count);
    fprintf(stderr,"Time for getting samples %f\n", tot_time);

    return 0;
}
