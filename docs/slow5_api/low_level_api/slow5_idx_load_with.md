# slow5_idx_load_with

## NAME
slow5_idx_load_with - loads the index file for a SLOW5 file given a file path for the index

## SYNOPSYS
`int slow5_idx_load_with(slow5_with_file_t *s5p, const char *pathname)`

## DESCRIPTION
`slow5_idx_load_with()` loads an index file for a SLOW5 file pointed by *s5p* into the memory from the disk and associates the index with *s5p*. If the index file is not found, the index is first created and written to the disk.

`slow5_idx_load_with()` could be called instead of `slow5_idx_load_with()` when the index file is at a custom location.

## RETURN VALUE
Upon successful completion, `slow5_idx_load_with()` returns a non-negative integer. Otherwise, a negative value is returned.

## NOTES
`slow5_errno` will be set in future to indicate the type of error that occurred.

## EXAMPLES

```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example.slow5"
#define INDEX_PATH "indexes/example.slow5.idx"


int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }
    int ret=0;

    ret = slow5_idx_load_with(sp, INDEX_PATH);
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

[slow5_idx_create()](../slow5_idx_create.md), [slow5_idx_unload()](../slow5_idx_unload.md), [slow5_idx_load()](../slow5_idx_load.md)
