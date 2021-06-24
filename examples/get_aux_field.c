#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example2.slow5"

int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }
    slow5_rec_t *rec = NULL;
    int ret=0;
    ret = slow5_get_next(&rec,sp);
    if(ret<0){
        fprintf(stderr,"Error in slow5_get_next. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }

    //------------------------------------------------------------------------
    //              get auxiliary values with primitive datatype
    //------------------------------------------------------------------------
    ret=0;
    double median_before = slow5_aux_get_double(rec,"median_before",&ret);
    if(ret!=0){
        fprintf(stderr,"Error in getting auxiliary attribute from the file. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"median_before = %f\n", median_before);

    uint64_t start_time = slow5_aux_get_uint64(rec, "start_time", &ret);
    if(ret!=0){
        fprintf(stderr,"Error in getting auxiliary attribute from the file. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"start_time = %lu\n", start_time);

    //------------------------------------------------------------------------
    //              get auxiliary values with array datatype
    //------------------------------------------------------------------------

    uint64_t len;
    char* channel_number = slow5_aux_get_string(rec, "channel_number", &len, &ret);
    if(ret!=0){
        fprintf(stderr,"Error in getting auxiliary attribute from the file. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"channel_number = %s\n", channel_number);

    slow5_rec_free(rec);
    slow5_close(sp);

}
