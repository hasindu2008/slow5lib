# slow5_init_batch

## NAME

slow5_init_batch - initialises a slow5 record batch

## SYNOPSYS

`slow5_batch_t* slow5_init_batch(int batch_capacity)`

## DESCRIPTION

`slow5_init_batch()` function initialises and returns a batch of empty SLOW5 records. The number of empty records in the batch will be what is specified by *batch_capacity*. The returned SLOW5 record batch must be later freed by calling `slow5_free_batch()`.


## RETURN VALUE

Upon successful completion, `slow5_init_batch()` returns a *slow5_batch_t* pointer.

## ERRORS

Currently, in case of error, this function will exit the programme. Proper error handling might be introduced in future, where a NULL return is indicative of error.

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

    // ...

    slow5_free_batch(read_batch);
    slow5_free_mt(mt);
    slow5_close(sp);

    return 0;
}
```

## SEE ALSO

[slow5_free_batch()](slow5_free_batch.md)
