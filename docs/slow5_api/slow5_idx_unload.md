# slow5_idx_unload

## NAME
slow5_idx_unload - unloads a SLOW5 index from the memory

## SYNOPSYS
`void slow5_idx_unload(slow5_file_t *s5p)`

## DESCRIPTION
`slow5_idx_unload()` unloads an index file loaded into memory for a SLOW5 file pointed by *s5p*, which must have been previously loaded using `slow5_idx_load()`. Otherwise, or if *slow5_idx_unload(s5p)* has already been called before, undefined behaviour occurs.

## RETURN VALUE

No return value


## NOTES


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

[slow5_idx_create()](slow5_idx_create.md), [slow5_idx_load()](slow5_idx_load.md)
