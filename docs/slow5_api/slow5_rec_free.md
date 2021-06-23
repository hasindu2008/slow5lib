# slow5_rec_free

## NAME
slow5_rec_free - frees up a SLOW5 record from memory

## SYNOPSYS
`void slow5_rec_free(slow5_rec_t *read)`

## DESCRIPTION

The `slow5_rec_free()` function frees the memory of a *slow5_rec_t* structure pointed by *read*, which must have been returned by a previous call to `slow5_get_next()` or `slow5_get()`. Otherwise, or if *free(read)* has already been called before, undefined behaviour occurs.
If *read* is NULL, no operation is performed.

## RETURN VALUE

None


## EXAMPLES

```
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
    slow5_rec_t *rec = NULL;
    int ret=0;

    ret = slow5_get_next(&rec,sp);
    if(ret<0){
        fprintf(stderr,"Error in slow5_get_next. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }

    slow5_rec_free(rec);

    slow5_close(sp);

}

```

## SEE ALSO

[slow5_get()](slow5_open.md), [slow5_get_next()](slow5_get_next.md)
