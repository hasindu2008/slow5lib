
// an example programme that uses slow5lib to obtain a slow5 header data attribute

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example.slow5"

int main(){

    //open the SLOW5 file
    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }

    const slow5_hdr_t* header = sp->header; //pointer to the SLOW5 header
    int read_group = 0;
    char* read_group_0_run_id_value = slow5_hdr_get("run_id", read_group, header); //get the value of the attribute "run_id" in the read group 0
    if(read_group_0_run_id_value != NULL ){ //check if the attribute exists and print the value
        printf("%s\n",read_group_0_run_id_value);
    }

    //close the SLOW5 file
    slow5_close(sp);

    return 0;
}
