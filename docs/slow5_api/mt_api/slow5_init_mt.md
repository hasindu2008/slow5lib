# slow5_init_mt

## NAME

slow5_init_mt - initialises a slow5 multi-thread structure

## SYNOPSYS

`slow5_mt_t *slow5_init_mt(int num_thread, slow5_file_t *s5p)`

## DESCRIPTION

`slow5_init_mt()` initialises and returns a slow5 multi-thread structure for the number of threads specified by *num_thread*. The argument *s5p* must to a *slow5_file_t* opened using `slow5_open()`. The returned slow5 multi-thread structure must be later freed by calling `slow5_free_mt()`.


## RETURN VALUE

Upon successful completion, `slow5_init_mt()` returns a slow5_mt_t pointer.


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

[slow5_free_mt()](slow5_free_mt.md)

