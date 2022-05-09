//get all the samples and sum them to stdout
//make zstd=1
//gcc -Wall -O2 -I include/ -o get_all_samples test/bench/get_all_samples.c lib/libslow5.a python/slow5threads.c -lm -lz -lzstd -lpthread  -fopenmp

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

    if(argc != 5) {
        fprintf(stderr, "Usage: %s in_file.blow5 out_file.csv num_thread batch_size\n", argv[0]);
        return EXIT_FAILURE;
    }

    slow5_rec_t **rec = NULL;
    int ret=1;
    int batch_size = atoi(argv[4]);
    int num_thread = atoi(argv[3]);
    omp_set_num_threads(num_thread);

    uint64_t *sums = malloc(sizeof(uint64_t)*batch_size);

    FILE *fp = fopen(argv[2],"w");
    if(fp==NULL){
        fprintf(stderr,"Error in opening file %s for writing\n",argv[2]);
        perror("perr: ");
        exit(EXIT_FAILURE);
    }
    fputs("read_id,samples\n", fp);


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
        tot_time += realtime() - t0;
        fprintf(stderr,"batch loaded with %d reads\n",ret);

        #pragma omp parallel for
        for(int i=0;i<ret;i++){
            uint64_t sum = 0;
            for(int j=0; j<rec[i]->len_raw_signal; j++){
                sum += rec[i]->raw_signal[j];
            }
            sums[i] = sum;
        }

        fprintf(stderr,"batch processed with %d reads\n",ret);

        for(int i=0;i<ret;i++){
            fprintf(fp,"%s,%ld\n",rec[i]->read_id,sums[i]);
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

    fclose(fp);

    fprintf(stderr,"Time for getting samples %f\n", tot_time);

}
