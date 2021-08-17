# slow5_idx_create

## NAME
slow5_idx_create - creates an index file for a SLOW5 file

## SYNOPSYS
`int slow5_idx_create(slow5_file_t *s5p)`

## DESCRIPTION
Creates an index file to a SLOW5 file pointed by *s5p* to enable random access (based on read ID) to the SLOW5 file.  Overwrites if the index file already exists.

## RETURN VALUE
Upon successful completion, `slow5_idx_create()` returns a non-negative integer. Otherwise, a negative value is returned.

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

    ret = slow5_idx_create(sp);
    if(ret<0){
        fprintf(stderr,"Error in creating index\n");
        exit(EXIT_FAILURE);
    }

    slow5_close(sp);

}
```

## SEE ALSO
[slow5_idx_load()](slow5_idx_load.md), [`slow5_idx_unload()`](slow5_idx_unload.md)
