// an example programme that uses slow5lib to randmly access records in a SLOW5 file using multiple threads (pthreads)

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>
#include <pthread.h>
#include <errno.h>

#define FILE_PATH "examples/example.slow5"
#define NUM_THREADS 4
#define READ_LIST_SIZE 4  //for simplicity number of threads are equal to number of reads to access, thus we assign one thread to each access

char * read_id_list[READ_LIST_SIZE] = {"r4", "r1", "r3", "r4"};


/* argument wrapper for the multithreaded framework used for data processing */
typedef struct {
    slow5_file_t *sp;
    char *read_id;
} pthread_arg;

void* access_read(void* voidargs) {
    pthread_arg *args = (pthread_arg*)voidargs;
    slow5_rec_t *rec = NULL;

    int ret = slow5_get(args->read_id, &rec, args->sp);
    if (ret < 0) {
        fprintf(stderr, "Error when when fetching the read %s\n",args->read_id);
    } else {
        fprintf(stderr, "Successfully fetched the read %s with %ld raw signal samples\n", rec->read_id, rec->len_raw_signal);
        //do the processing here as needed
    }
    slow5_rec_free(rec);

    pthread_exit(0);
}


int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }

    int ret,t = 0;

    ret = slow5_idx_load(sp);
    if (ret < 0) {
        fprintf(stderr, "Error in loading index\n");
        exit(EXIT_FAILURE);
    }

    //threads
    pthread_t tids[NUM_THREADS];
    pthread_arg args[NUM_THREADS];

    //create threads
    for(t = 0; t < NUM_THREADS; t++){
        args[t].sp = sp;
        args[t].read_id = read_id_list[t];

        ret = pthread_create(&tids[t], NULL, access_read,
                             (void*)(&args[t]));
        if(ret < 0){
            fprintf(stderr, "Error creating thread\n");
            exit(EXIT_FAILURE);
        }
    }

    //pthread joining
    for (t = 0; t < NUM_THREADS; t++) {
        int ret = pthread_join(tids[t], NULL);
        if(ret < 0){
            fprintf(stderr, "Error creating thread\n");
            exit(EXIT_FAILURE);
        }
    }

    slow5_idx_unload(sp);
    slow5_close(sp);

    return  0;
}
