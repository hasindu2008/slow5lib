# slow5_get_next_batch

## NAME

slow5_get_next_batch - fetches the next batch of records at the current file pointer of a SLOW5 file

## SYNOPSYS

`int slow5_get_next_batch(slow5_mt_t *mt, slow5_batch_t *read_batch, int num_reads)`

## DESCRIPTION

`slow5_get_next_batch()` fetches the next *num_reads* number of records from a SLOW5 file associated with *mt* at the current file pointer into a *slow5_batch_t*.

## RETURN VALUE

The number of records fetched.

## NOTES

*num_reads* must be less than or equal to the capacity of the read_batch.

## ERRORS

Currently, in case of error, this function will exit the programme. Proper error handling might be introduced in future, where a negative number return is indicative of error.

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
    int batch_size = 4096;
    int num_thread = 8;

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

    while((ret = slow5_get_next_batch(mt,read_batch,4096)) > 0){

        for(int i=0;i<ret;i++){
            rec = read_batch->slow5_rec;
            uint64_t len_raw_signal = rec[i]->len_raw_signal;
            printf("%s\t%ld\n",rec[i]->read_id,len_raw_signal);
        }

        if(ret<4096){ //this indicates nothing left to read //better handling of errors may be introduced in future
            break;
        }
    }

    slow5_free_batch(read_batch);
    slow5_free_mt(mt);
    slow5_close(sp);

    return 0;
}
```

## SEE ALSO

[slow5_get()](slow5_get_batch.md)