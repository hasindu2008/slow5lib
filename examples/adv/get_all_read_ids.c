// an example programme that uses slow5lib to get all the read ids and print them to stdout

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

int main(int argc, char *argv[]) {

    if(argc != 2) {
        fprintf(stderr, "Usage: %s in_file.blow5\n", argv[0]);
        return EXIT_FAILURE;
    }

    slow5_file_t *sp = slow5_open(argv[1],"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }

    int ret=0;
    ret = slow5_idx_load(sp);
    if(ret<0){
        fprintf(stderr,"Error in loading index\n");
        exit(EXIT_FAILURE);
    }

    uint64_t num_reads = 0;
    char **read_ids = slow5_get_rids(sp, &num_reads);
    if(read_ids==NULL){
        fprintf(stderr,"Error in getting list of read IDs\n");
        exit(EXIT_FAILURE);
    }

    for(uint64_t i=0; i<num_reads; i++) {
        fputs(read_ids[i],stdout);
        fputc('\n',stdout);
    }

    slow5_idx_unload(sp);

    slow5_close(sp);

    return 0;
}
