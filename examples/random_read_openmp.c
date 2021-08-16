
// an example programme that uses slow5lib to randmly access records in a SLOW5 file using multiple threads (openMP)
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example.slow5"
#define READ_LIST_SIZE 4

char * read_id_list[READ_LIST_SIZE] = {"r4", "r1", "r3", "r4"};

int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }

    int ret=0;
    ret = slow5_idx_load(sp);
    if (ret < 0) {
        fprintf(stderr, "Error in loading index\n");
        exit(EXIT_FAILURE);
    }

    #pragma omp parallel for
    for(int32_t i=0; i<READ_LIST_SIZE; i++) {
        slow5_rec_t *rec = NULL;
        ret = slow5_get(read_id_list[i], &rec, sp);
        if (ret < 0) {
            fprintf(stderr, "Error in when fetching the read %s\n",read_id_list[i]);
        } else {
            fprintf(stderr, "Successfully fetched the read %s with %ld raw signal samples\n", rec->read_id, rec->len_raw_signal);
        }
        slow5_rec_free(rec);
    }

    slow5_idx_unload(sp);

    slow5_close(sp);

}
