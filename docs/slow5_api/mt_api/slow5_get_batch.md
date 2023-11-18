# slow5_get_batch

## NAME

slow5_get_batch - fetches a batch of SLOW5 records corresponding to a given batch of read ID

## SYNOPSYS

`int slow5_get_batch(slow5_mt_t *mt, slow5_batch_t *read_batch, char **rid, int num_rid)`

## DESCRIPTION

`slow5_get_batch()` fetches a batch of record from a SLOW5 file associated with *mt* for a specified list of read IDs. The list of read must be pointed by *rid* and the the size of the list must be specified by *num_rid*.

## RETURN VALUE

The number of records fetched.

## NOTES

*num_rid* must be less than or equal to the capacity of the read_batch.

## ERRORS

Currently, in case of error, this function will exit the programme. Proper error handling might be introduced in future, where a negative number return is indicative of error.

## NOTES


## EXAMPLES

```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>
#include <slow5/slow5_mt.h>

#define FILE_PATH "examples/example.slow5"

int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }

    slow5_rec_t **rec = NULL;
    int ret=0;

    ret = slow5_idx_load(sp);
    if(ret<0){
        fprintf(stderr,"Error in loading index\n");
        exit(EXIT_FAILURE);
    }

    int num_rid = 4;
    int num_thread = 2;
    int batch_size = 4096;
    char *rid[num_rid];
    rid[0]="r4";
    rid[1]="r1",
    rid[2]="r2";
    rid[3]="r5";

    slow5_mt_t *mt = slow5_init_mt(num_thread,sp);
    if (mt==NULL){ //currently not useful, but better have this for future proofing
        fprintf(stderr,"Error in initialising multi-thread struct\n");
        exit(EXIT_FAILURE);
    }

    slow5_batch_t *read_batch = slow5_init_batch(batch_size);
    if (read_batch==NULL){ //currently not useful, but better have this for future proofing
        fprintf(stderr,"Error in initialising slow5 record batch\n");
        exit(EXIT_FAILURE);
    }

    ret = slow5_get_batch(mt, read_batch, rid, num_rid);
    if(ret!=num_rid){
        fprintf(stderr,"Error in fetching all records\n");
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<ret;i++){
        rec = read_batch->slow5_rec;
        uint64_t len_raw_signal = rec[i]->len_raw_signal;
        printf("%s\t%ld\n",rec[i]->read_id,len_raw_signal);
    }

    slow5_free_batch(read_batch);
    slow5_free_mt(mt);

    slow5_idx_unload(sp);
    slow5_close(sp);

    return 0;
}
```

## SEE ALSO

[slow5_get()](slow5_get_next_batch.md)