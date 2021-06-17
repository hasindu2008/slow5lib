
// TODO: add to automated tests

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <slow5/slow5.h>

#define FILE_PATH "test/data/exp/aux_array/exp_lossless.slow5"


int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }

    uint64_t len=0;
    const char **list=slow5_get_hdr_keys(sp->header,&len);

    for(int i=0;i<len;i++){
        printf("%s\t",list[i]);
    }
    printf("\n");
    free(list);


    char **list2 = slow5_get_aux_names(sp->header,&len);
    for(int i=0;i<len;i++){
        printf("%s\t",list2[i]);
    }

    slow5_close(sp);

}
