//get all the samples and sum them to stdout
//make zstd=1
//gcc -Wall -O2 -I include/ -o get_all_samples test/bench/get_all_samples.c lib/libslow5.a python/slow5threads.c -lm -lz -lzstd -lpthread  -fopenmp

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>
#include <omp.h>
#include <sys/time.h>
#include <pthread.h>
#include "../../python/slow5threads.h"
#include "../../src/slow5_extra.h"

int threads = 10;

/* argument wrapper for multithreaded framework used for input/processing/output interleaving */
typedef struct {
    int batch_size;
    slow5_rec_t **slow5_rec;
    uint64_t *sums;
    char **mem_records;
    size_t *mem_bytes;
    slow5_file_t *sp;
    FILE *fp;
    //conditional variable for notifying the processing to the output threads
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    int8_t finished;
} pthread_arg_t;


static inline double realtime(void) {
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return tp.tv_sec + tp.tv_usec * 1e-6;
}


/* load a data batch from disk */
int load_raw_batch(char ***mem_records_a, size_t **mem_bytes_a, slow5_file_t *sf, int batch_size) {

    char **mem_records = (char **)malloc(batch_size * sizeof(char *));
    size_t *mem_bytes = (size_t *)malloc(batch_size * sizeof(size_t));


    int32_t i = 0;
    while (i < batch_size) {
        mem_records[i] = (char *)slow5_get_next_mem(&(mem_bytes[i]), sf);

        if (mem_records[i] == NULL) {
            if (slow5_errno != SLOW5_ERR_EOF) {
                fprintf(stderr,"Error reading from SLOW5 file %d\n", slow5_errno);
                exit(EXIT_FAILURE);
            }
            else {
                break;
            }
        }
        else {
            i++;
        }
    }

    *mem_records_a = mem_records;
    *mem_bytes_a = mem_bytes;

    return i;
}

void free_raw_batch(char **mem_records, size_t *mem_bytes, int batch_size) {
    for(int i=0;i<batch_size;i++) {
        free(mem_records[i]);
    }
    free(mem_records);
    free(mem_bytes);

}


void* process_batch(void* voidargs) {

    pthread_arg_t* args = (pthread_arg_t*)voidargs;
    int batch_size = args->batch_size;
    slow5_rec_t **slow5_rec = (slow5_rec_t**)calloc(batch_size,sizeof(slow5_rec_t*));
    uint64_t *sums = (uint64_t*)malloc(batch_size*sizeof(uint64_t));
    char **mem_records = args-> mem_records;
    size_t *mem_bytes = args->mem_bytes;
    slow5_file_t *sp = args->sp;

    //fprintf(stderr,"Processing started for %d records\n", batch_size);

    omp_set_num_threads(threads);

    #pragma omp parallel for
    for(int i=0;i<batch_size;i++){

        if(slow5_rec_depress_parse(&mem_records[i], &mem_bytes[i], NULL, &slow5_rec[i], sp)!=0){
            fprintf(stderr,"Error parsing the record %s",slow5_rec[i]->read_id);
            exit(EXIT_FAILURE);
        }
        uint64_t sum = 0;
        for(int j=0; j<slow5_rec[i]->len_raw_signal; j++){
            sum += slow5_rec[i]->raw_signal[j];
        }
        sums[i] = sum;
    }
    args->slow5_rec = slow5_rec;
    args->sums = sums;

    pthread_mutex_lock(&args->mutex);
    pthread_cond_signal(&args->cond);
    args->finished=1;
    pthread_mutex_unlock(&args->mutex);

    //fprintf(stderr,"batch processed with %d reads\n",batch_size);

    pthread_exit(0);
}

void* output_free_batch(void* voidargs){

    pthread_arg_t* args = (pthread_arg_t*)voidargs;
    //fprintf(stderr,"Waiting till processor complete\n");

    pthread_mutex_lock(&args->mutex);
    while(args->finished==0){
        pthread_cond_wait(&args->cond, &args->mutex);
    }
    pthread_mutex_unlock(&args->mutex);

    //fprintf(stderr,"processor completed\n");


    int batch_size = args->batch_size;
    slow5_rec_t **slow5_rec = args->slow5_rec;
    uint64_t *sums = args->sums;
    char **mem_records = args-> mem_records;
    size_t *mem_bytes = args->mem_bytes;
    FILE *fp = args->fp;


    for(int i=0;i<batch_size;i++){
        fprintf(fp,"%s,%ld\n",slow5_rec[i]->read_id,sums[i]);
    }
    //fprintf(stderr,"batch printed with %d reads\n",batch_size);
    free_raw_batch(mem_records, mem_bytes, batch_size);
    for(int i=0;i<batch_size;i++){
        slow5_rec_free(slow5_rec[i]);
    }
    free(slow5_rec);
    free(sums);
    pthread_cond_destroy(&args->cond);
    pthread_mutex_destroy(&args->mutex);

    free(args);

    pthread_exit(0);
}



int main(int argc, char *argv[]) {

    if(argc != 5) {
        fprintf(stderr, "Usage: %s in_file.blow5 out_file.csv num_thread batch_size\n", argv[0]);
        return EXIT_FAILURE;
    }


    int batch_size = atoi(argv[4]);
    int num_thread = atoi(argv[3]);
    int ret=batch_size;
    threads = num_thread;


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

    int8_t first_flag_p=0;
    int8_t first_flag_pp=0;
    pthread_t tid_p;
    pthread_t tid_pp;


    while(ret == batch_size){

        t0 = realtime();
        char **mem_records = NULL;
        size_t *mem_bytes = NULL;
        ret = load_raw_batch(&mem_records, &mem_bytes, sp, batch_size);
        tot_time += realtime() - t0;
        //fprintf(stderr,"batch loaded with %d reads\n",ret);

        if(first_flag_p){
            if(pthread_join(tid_p, NULL) < 0){
                fprintf(stderr,"Error in joining thread\n");
                exit(EXIT_FAILURE);
            }
        }
        first_flag_p=1;

        pthread_arg_t *pt_arg = (pthread_arg_t*)malloc(sizeof(pthread_arg_t));
        pt_arg->batch_size = ret;
        pt_arg->mem_records = mem_records;
        pt_arg->mem_bytes = mem_bytes;
        pt_arg->sp = sp;
        pt_arg->fp = fp;
        pt_arg->finished = 0;


        if (pthread_cond_init(&pt_arg->cond, NULL) < 0){
            fprintf(stderr,"Error in initializing condition variable\n");
            exit(EXIT_FAILURE);
        }
        if (pthread_mutex_init(&pt_arg->mutex, NULL) < 0){
            fprintf(stderr,"Error in initializing mutex\n");
            exit(EXIT_FAILURE);
        }

        if(pthread_create(&tid_p, NULL, process_batch,
                                (void*)(pt_arg)) < 0) {
            fprintf(stderr,"Error in creating thread\n");
            exit(EXIT_FAILURE);
        }

        if(first_flag_pp){
            if(pthread_join(tid_pp, NULL) < 0){
                fprintf(stderr,"Error in joining thread\n");
                exit(EXIT_FAILURE);
            }
        }
        first_flag_pp=1;

        if(pthread_create(&tid_pp, NULL, output_free_batch,
                                (void*)(pt_arg)) < 0){
            fprintf(stderr,"Error in creating thread\n");
            exit(EXIT_FAILURE);
        }


    }

    if(pthread_join(tid_p, NULL) < 0){
        fprintf(stderr,"Error in joining thread\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_join(tid_pp, NULL) < 0){
        fprintf(stderr,"Error in joining thread\n");
        exit(EXIT_FAILURE);
    }

    t0 = realtime();
    slow5_close(sp);
    tot_time += realtime() - t0;

    fclose(fp);

    fprintf(stderr,"Time for getting raw bytes (exclude depress & parse) %f\n", tot_time);

    return 0;
}
