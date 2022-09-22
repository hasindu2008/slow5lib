// an example programme that uses slow5lib to sequentially read raw SLOW5 records using one thread
// and decode records in prallel using multiple threads (openMP)

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example.slow5"

#define READ_BUFFER_SIZE 1024 //number of reads to be loaded in one go

int main(){

    //open the SLOW5 file
    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }

    //for simplicity assume file has less than READ_BUFFER_SIZE reads
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

    //decode a batch of raw records using multiple threads in parallel
    #pragma omp parallel for
    for(int i=0; i<n_rec; i++) {
        slow5_rec_t *rec = NULL; //slow5 record to put the decoded record
        int ret = slow5_decode(&mem[i], &bytes[i], &rec, sp); //decode the raw record
        if (ret < 0) {
            fprintf(stderr, "Error in when decoding the read %d\n",i);
        } else {
            fprintf(stderr, "Successfully decoded the read %s with %ld raw signal samples\n", rec->read_id, rec->len_raw_signal);
        }
        slow5_rec_free(rec); //free the SLOW5 record
        free(mem[i]); //free the raw record
    }
    fprintf(stderr, "Decoded %d raw records\n", n_rec);

    //close the SLOW5 file
    slow5_close(sp);

    return 0;
}
