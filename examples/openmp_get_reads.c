
// an example programme that uses slow5lib to read a SLOW5 file sequentially
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example.slow5"

#define TO_PICOAMPS(RAW_VAL,DIGITISATION,OFFSET,RANGE) (((RAW_VAL)+(OFFSET))*((RANGE)/(DIGITISATION)))

int main(){
    int32_t num_rec = 4;
    char * read_ids[40] = {"r1", "r2", "r3", "r4", "r5"};

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

    slow5_rec_t *rec = NULL;
    #pragma omp parallel for
    for(int32_t i=0; i<num_rec; i++) {
        ret = slow5_get(read_ids[i], &rec, sp);
        if (ret < 0) {
            fprintf(stderr, "Error in when fetching the read\n");
        } else {
            printf("thread%d read-%s\n", omp_get_thread_num(), rec->read_id);
//            uint64_t len_raw_signal = rec->len_raw_signal;
//            for (uint64_t i = 0; i < len_raw_signal; i++) {
//                double pA = TO_PICOAMPS(rec->raw_signal[i], rec->digitisation, rec->offset, rec->range);
//            printf("%f ",pA);
//            }
//        printf("\n");
        }
    }
    slow5_rec_free(rec);

    slow5_idx_unload(sp);

    slow5_close(sp);

}
