// an example programme that uses slow5lib to obtain auxiliary field of enum type from a slow5/blow5 file

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/adv/example3.blow5"
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

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

    uint8_t num_label = 0;
    char **labels = slow5_get_aux_enum_labels(sp->header, "end_reason", &num_label);
    if(labels==NULL){
        fprintf(stderr,"Error in getting list of enum labels\n");
        exit(EXIT_FAILURE);
    }

    uint8_t end_reason = slow5_aux_get_enum(rec,"end_reason",&ret);
    if(ret!=0){
        fprintf(stderr,"Error in getting auxiliary attribute from the file. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }
    if(end_reason != SLOW5_ENUM_NULL){
        printf("end_reason = %s\n", labels[end_reason]);
    } else{
        printf("end_reason is missing for the record\n");
    }

    slow5_rec_free(rec);
    slow5_close(sp);

    return 0;
}
