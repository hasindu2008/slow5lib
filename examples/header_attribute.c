
// an example programme that uses slow5lib to obtain a slow5 header attribute

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example.slow5"

int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }
    const slow5_hdr_t* header = sp->header;
    int read_group = 0;
    char* read_group_0_run_id_value = slow5_hdr_get("run_id", read_group, header);
    if(read_group_0_run_id_value != NULL ){
        printf("%s\n",read_group_0_run_id_value);
    }

    slow5_close(sp);

}
