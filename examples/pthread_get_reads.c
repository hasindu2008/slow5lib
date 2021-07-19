// an example programme that uses slow5lib to read a SLOW5 file using multiple threads

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>
#include <pthread.h>
#include <errno.h>

#define FILE_PATH "examples/example.slow5"
#define TO_PICOAMPS(RAW_VAL,DIGITISATION,OFFSET,RANGE) (((RAW_VAL)+(OFFSET))*((RANGE)/(DIGITISATION)))

static inline void neg_chk(int ret, const char* func, const char* file,
                           int line) {
    if (ret < 0) {
        fprintf(stderr, "[%s] Unexpected negative value : %s.", func, strerror(errno));
        fprintf(stderr, "[%s] Error occured at %s:%d.", func, file, line);

        exit(EXIT_FAILURE);
    }
}

/* argument wrapper for the multithreaded framework used for data processing */
typedef struct {
    int32_t starti;
    int32_t endi;
    int32_t thread_index;
    slow5_file_t *sp;
    char **read_ids;
} pthread_arg;

void get_record(pthread_arg* pthreadArg){
    slow5_rec_t *rec = NULL;
    int ret=0;
    for(int32_t i=pthreadArg->starti; i<pthreadArg->endi; i++) {
        ret = slow5_idx_load(pthreadArg->sp);
        if (ret < 0) {
            fprintf(stderr, "Error in loading index\n");
            exit(EXIT_FAILURE);
        }
        ret = slow5_get(pthreadArg->read_ids[i], &rec, pthreadArg->sp);
        if (ret < 0) {
            fprintf(stderr, "Error in when fetching the read\n");
        } else {
            printf("thread%d read-%s\n", pthreadArg->thread_index, rec->read_id);
//            uint64_t len_raw_signal = rec->len_raw_signal;
//            for (uint64_t i = 0; i < len_raw_signal; i++) {
//                double pA = TO_PICOAMPS(rec->raw_signal[i], rec->digitisation, rec->offset, rec->range);
//            printf("%f ",pA);
//            }
//        printf("\n");
        }
    }
    slow5_rec_free(rec);
}
void* pthread_single_merge(void* voidargs) {
    pthread_arg* args = (pthread_arg*)voidargs;
    get_record(args);
    pthread_exit(0);
}
void pthread_data(int num_thread, int num_rec, slow5_file_t *sp, char** read_ids) {
    //create threads
    pthread_t tids[num_thread];
    pthread_arg pt_args[num_thread];
    int32_t t, ret;
    int32_t i = 0;
    int32_t step = (num_rec + num_thread - 1) / num_thread;

    //set the data structures
    for (t = 0; t < num_thread; t++) {
        pt_args[t].starti = i;
        i += step;
        if (i > num_rec) {
            pt_args[t].endi = num_rec;
        } else {
            pt_args[t].endi = i;
        }
        pt_args[t].thread_index=t;
        pt_args[t].sp = sp;
        pt_args[t].read_ids = read_ids;
    }

    //create threads
    for(t = 0; t < num_thread; t++){
        ret = pthread_create(&tids[t], NULL, pthread_single_merge,
                             (void*)(&pt_args[t]));
        neg_chk(ret, __func__ , __FILE__, __LINE__);
    }

    //pthread joining
    for (t = 0; t < num_thread; t++) {
        int ret = pthread_join(tids[t], NULL);
        neg_chk(ret, __func__ , __FILE__, __LINE__);
    }
}

int main(){
    int32_t num_thread = 2;
    int32_t num_rec = 4;
    char * read_ids[40] = {"r1", "r2", "r3", "r4", "r5"};

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }

    // call threads
    pthread_data(num_thread, num_rec, sp, read_ids);

    slow5_idx_unload(sp);
    slow5_close(sp);
    return  0;
}
