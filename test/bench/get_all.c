//get all the samples and sum them to stdout
//make zstd=1
//gcc -Wall -O2 -I include/ -o get_all test/bench/get_all.c lib/libslow5.a python/slow5threads.c -lm -lz -lzstd -lpthread

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>
#include <omp.h>
#include <sys/time.h>
#include <slow5/slow5_mt.h>

static inline double realtime(void) {
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return tp.tv_sec + tp.tv_usec * 1e-6;
}


int main(int argc, char *argv[]) {

    if(argc != 5) {
        fprintf(stderr, "Usage: %s reads.blow5 num_thread batch_size\n", argv[0]);
        return EXIT_FAILURE;
    }

    slow5_rec_t **rec = NULL;
    int ret=1;
    int batch_size = atoi(argv[4]);
    int num_thread = atoi(argv[3]);

    FILE *fp = stdout;
    fputs("#read_id\tsample_sum\n", fp);

    int64_t sum_samples = 0;


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
        ret = slow5_get_next_batch_lazy(&rec,sp,batch_size,num_thread);
        tot_time += realtime() - t0;
        fprintf(stderr,"batch loaded with %d reads\n",ret);

        for(int i=0; i<ret; i++){
            sum_samples += rec[i]->len_raw_signal;
        }

        t0 = realtime();
        slow5_free_batch_lazy(&rec,ret);
        tot_time += realtime() - t0;

        if(ret<batch_size){ //this indicates nothing left to read //need to handle errors
            break;
        }
    }

    t0 = realtime();
    slow5_close(sp);
    tot_time += realtime() - t0;

    fprintf(stderr,"Time for getting all records %f\n", tot_time);
    fprintf(stderr,"%f MSamples/s\n", (double)sum_samples/(tot_time*1000*1000));

    return 0;
}
