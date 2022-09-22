// an example programme that uses slow5lib to randomly access records in a SLOW5 file

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example.slow5"

#define TO_PICOAMPS(RAW_VAL,DIGITISATION,OFFSET,RANGE) (((RAW_VAL)+(OFFSET))*((RANGE)/(DIGITISATION)))

int main(){

    //open the SLOW5 file
    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }

    slow5_rec_t *rec = NULL; //slow5 record to be read
    int ret=0; //for return value

    //load the SLOW5 index (will be built if not present)
    ret = slow5_idx_load(sp);
    if(ret<0){
        fprintf(stderr,"Error in loading index\n");
        exit(EXIT_FAILURE);
    }

    //fetch the read with read_id "r3"
    ret = slow5_get("r3", &rec, sp);
    if(ret<0){
        fprintf(stderr,"Error in when fetching the read\n");
    }
    else{
        printf("%s\t",rec->read_id);
        uint64_t len_raw_signal = rec->len_raw_signal;
        for(uint64_t i=0;i<len_raw_signal;i++){ //iterate through the raw signal and print in picoamperes
            double pA = TO_PICOAMPS(rec->raw_signal[i],rec->digitisation,rec->offset,rec->range);
            printf("%f ",pA);
        }
        printf("\n");
    }

    //..... fetch any other read using slow5_get (as above)

    //free the SLOW5 record
    slow5_rec_free(rec);

    //free the SLOW5 index
    slow5_idx_unload(sp);

    //close the SLOW5 file
    slow5_close(sp);

    return 0;
}
