# slow5_free_mt

## NAME

slow5_free_mt - frees up a slow5 multi-thread structure

## SYNOPSYS

`void slow5_free_mt(slow5_mt_t *mt)`

## DESCRIPTION

The `slow5_free_mt()` function frees the memory of a slow5_mt_t structure pointed by mt, which must have been returned by a previous call to `slow5_init_mt()`.

## RETURN VALUE

None

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

    int num_thread = 8;

    slow5_mt_t *mt = slow5_init_mt(num_thread,sp);
    if (mt==NULL){ //currently not useful, but better have this for future proofing
        fprintf(stderr,"Error in initialising multi-thread struct\n");
        exit(EXIT_FAILURE);
    }

    //...

    slow5_free_mt(mt);
    slow5_close(sp);

    return 0;
}
```


## SEE ALSO

[slow5_init_mt()](slow5_init_mt.md)