// an example programme that uses slow5lib to sequentially read raw SLOW5 records using one thread
// and decode records in prallel using multiple threads (pthreads)

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>
#include <pthread.h>
#include <errno.h>

#define FILE_PATH "examples/example.slow5"
#define NUM_THREADS 5 //for simplicity assume number of threads is equal to number of reads in the file

#define READ_BUFFER_SIZE 1024 //number of reads to be loaded in one go

/* arguments for a thread */
typedef struct {
    slow5_file_t *sp;
    char *mem;
    size_t bytes;
} pthread_arg;

/* thread function */
void* access_read(void* voidargs) {

    pthread_arg *args = (pthread_arg*)voidargs; //cast the void pointer to pthread_arg pointer
    slow5_rec_t *rec = NULL; ////slow5 record to put the decoded record

    //decode the read assigned to this thread
    int ret = slow5_decode(&args->mem, &args->bytes, &rec, args->sp); //decode the raw record
    if (ret < 0) {
        fprintf(stderr, "Error in when decoding the read\n");
    } else {
        fprintf(stderr, "Successfully decoded the read %s with %ld raw signal samples\n", rec->read_id, rec->len_raw_signal);
        // .... do the processing here as needed
    }

    //free the SLOW5 record
    slow5_rec_free(rec);

    //free the raw record
    free(args->mem);

    pthread_exit(0);
}


int main(){

    //open the SLOW5 file
    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }

    int ret,t = 0; //for return value and thread index

    //for simplicity assume the file has less than READ_BUFFER_SIZE reads
    // and that the number of reads is equal to NUM_THREADS
    char *mem[READ_BUFFER_SIZE] = {NULL}; //memory to store raw records
    size_t bytes[READ_BUFFER_SIZE] = {0}; //memory to store raw record sizes

    int n_rec = 0; //number of records read

    //read a batch of raw records
    for(n_rec=0;n_rec<READ_BUFFER_SIZE;n_rec++){
        if(slow5_get_next_bytes(&mem[n_rec],&bytes[n_rec],sp)<0){
            if (slow5_errno != SLOW5_ERR_EOF) { //check if end of file has been reached (break the loop) or some other error (exit with error)
                fprintf(stderr, "Error reading from SLOW5 file %d", slow5_errno);
                exit(EXIT_FAILURE);
            } else {
                break;
            }
        }
    }
    fprintf(stderr, "Read %d raw records\n", n_rec);

    //thread IDs and arguments
    pthread_t tids[NUM_THREADS];
    pthread_arg args[NUM_THREADS];

    //create threads
    for(t = 0; t < NUM_THREADS; t++){
        args[t].sp = sp; //assign the SLOW5 file pointer
        args[t].mem = mem[t]; //assign a raw read record to each thread
        args[t].bytes = bytes[t]; //assign the size of the raw record to each thread

        ret = pthread_create(&tids[t], NULL, access_read, (void*)(&args[t]));
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

    //close the SLOW5 file
    slow5_close(sp);

    return  0;
}
