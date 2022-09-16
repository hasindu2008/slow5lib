// an example programme that uses slow5lib to obtain auxiliary field from a slow5/blow5 file

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example2.slow5"

int main(){

    //open the SLOW5 file
    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }

    slow5_rec_t *rec = NULL; //slow5 record to be read
    int ret=0; //for return value

    //get the very first record
    ret = slow5_get_next(&rec,sp);
    if(ret<0){
        fprintf(stderr,"Error in slow5_get_next. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }

    //------------------------------------------------------------------------
    //   getting auxiliary fields whose values are primitive datatypes
    //------------------------------------------------------------------------

    //median_before auxiliary field - double data type
    double median_before = slow5_aux_get_double(rec,"median_before",&ret);
    if(ret!=0){
        fprintf(stderr,"Error in getting auxiliary attribute from the file. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }
    if(!isnan(median_before)){  //SLOW5_DOUBLE_NULL is the generic NaN value returned by nan("""") and thus median_before != SLOW5_DOUBLE_NULL is not correct
        printf("median_before = %f\n", median_before);
    } else {
        printf("median_before is missing for the record\n");
    }

    //start_time auxiliary field - uint64 data type
    uint64_t start_time = slow5_aux_get_uint64(rec, "start_time", &ret);
    if(ret!=0){
        fprintf(stderr,"Error in getting auxiliary attribute from the file. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }
    if(start_time != SLOW5_UINT64_T_NULL){ //check if the field value is marked missing and print the value
        printf("start_time = %lu\n", start_time);
    } else{
        printf("start_time is missing for the record\n");
    }

    //------------------------------------------------------------------------
    //    getting auxiliary fields whose values are array datatypes
    //------------------------------------------------------------------------

    //channel_number auxiliary field - string (char *) datatype
    uint64_t len; //length of the array
    char* channel_number = slow5_aux_get_string(rec, "channel_number", &len, &ret);
    if(ret!=0){
        fprintf(stderr,"Error in getting auxiliary attribute from the file. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }
    if (channel_number != NULL){ //check if the field value exists and print the value
        printf("channel_number = %s\n", channel_number);
    } else{
        printf("channel_number is missing for the record\n");
    }

    //free the SLOW5 record
    slow5_rec_free(rec);

    //close the SLOW5 file
    slow5_close(sp);

    return 0;
}
