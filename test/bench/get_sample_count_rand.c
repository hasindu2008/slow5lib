//get all the samples and sum them to stdout
//make zstd=1 slow5_mt=1
//gcc -Wall -O2 -I include/ -o get_sample_count_rand test/bench/get_sample_count_rand.c lib/libslow5.a -lm -lz -lzstd -lpthread

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
        fprintf(stderr, "Usage: %s reads.blow5 inread_id.tsv num_thread batch_size\n", argv[0]);
        return EXIT_FAILURE;
    }

    int ret=0;
    int batch_size = atoi(argv[4]);
    int num_thread = atoi(argv[3]);

    uint64_t *sums = malloc(sizeof(uint64_t)*batch_size);

    char tmp[1024];
    FILE *fpr = fopen(argv[2],"r");
    if(fpr==NULL){
        fprintf(stderr,"Error in opening file %s for reading\n",argv[2]);
        perror("perr: ");;
        exit(EXIT_FAILURE);
    }

    FILE *fpw = stdout;
    fputs("#read_id\tsample_count\n", fpw);

    double tot_time = 0;
    double t0 = realtime();

    slow5_rec_t **rec = NULL;
    slow5_file_t *sp = slow5_open(argv[1],"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }

    ret = slow5_idx_load(sp);
    if(ret<0){
        fprintf(stderr,"Error in loading index\n");
        exit(EXIT_FAILURE);
    }

    tot_time += realtime() - t0;

    char **rid = malloc(sizeof(char*)*batch_size);

    while(1){

        int i=0;
        for(i=0; i<batch_size; i++){
            if (fscanf(fpr,"%s",tmp) < 1) {
                break;
            }
            rid[i] = strdup(tmp);
        }
        int num_rid = i;

        t0 = realtime();
        ret = slow5_get_batch_lazy(&rec, sp, rid, num_rid, num_thread);
        tot_time += realtime() - t0;

        if(ret!=num_rid){
            fprintf(stderr,"Error in getting batch\n");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr,"batch loaded with %d reads\n",ret);

        for(int i=0;i<ret;i++){
            fprintf(fpw,"%s\t%ld\n",rec[i]->read_id,rec[i]->len_raw_signal);
        }
        fprintf(stderr,"batch printed with %d reads\n",ret);

        t0 = realtime();
        slow5_free_batch_lazy(&rec,ret);
        tot_time += realtime() - t0;

        for(int i=0; i<num_rid; i++){
            free(rid[i]);
        }

        if(num_rid<batch_size){ //this indicates nothing left to read
            break;
        }

    }

    t0 = realtime();
    slow5_idx_unload(sp);
    slow5_close(sp);
    tot_time += realtime() - t0;

    free(sums);
    fclose(fpr);

    free(rid);

    fprintf(stderr,"Time for getting samples %f\n", tot_time);

    return 0;

}
