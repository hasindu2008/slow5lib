# slow5_idx_load

## NAME
slow5_idx_load - loads the index file for a SLOW5 file

## SYNOPSYS
`int slow5_idx_load(slow5_file_t *s5p)`

## DESCRIPTION
`slow5_idx_load()` loads an index file for a SLOW5 file pointed by *s5p* into the memory from the disk and associates the index with *s5p*. If the index file is not found, the index is first created and written to the disk.

`slow5_idx_load()` should be called successfully before using `slow5_get()`.

## RETURN VALUE
Upon successful completion, `slow5_idx_load()` returns a non-negative integer. Otherwise, a negative value is returned.

## NOTES
`slow5_errno` will be set in future to indicate the type of error that occurred.

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
    int ret=0;

    ret = slow5_idx_load(sp);
    if(ret<0){
        fprintf(stderr,"Error in loading index\n");
        exit(EXIT_FAILURE);
    }

    //...

    slow5_idx_unload(sp);

    slow5_close(sp);

}
```

## SEE ALSO

[slow5_idx_create()](slow5_idx_create.md), [slow5_idx_unload()](slow5_idx_unload.md)
