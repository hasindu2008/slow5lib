# slow5_get_rids

## NAME

slow5_get_rids - gets the pointer to the list of read IDs associated with a SLOW5 file

## SYNOPSYS

`char **slow5_get_rids(const slow5_file_t *s5p, uint64_t *len)`

## DESCRIPTION

`slow5_get_rids()` gets the pointer to the list of read IDs associated with a SLOW5 file pointed by *s5p*.  The SLOW5 index should have been already loaded before by calling `slow5_idx_load()`.
The number of reads will be set on the address specified by *len*.

## RETURN VALUE

Upon successful completion, `slow5_get_rids()` returns a *char*** pointer. Otherwise, a negative value is returned that indicates the error and `slow5_errno` is set to indicate the error.

## ERRORS

* `SLOW5_ERR_NOIDX`
    &nbsp;&nbsp;&nbsp;&nbsp; SLOW5 index has not been loaded.
* `SLOW5_ERR_OTH`:
	&nbsp;&nbsp;&nbsp;&nbsp; Other error.


## NOTES

`slow5_get_rids()` returns the list of read IDs stored in the index. Thus, returned pointer is from an internal data structure and must NOT be freed by user.

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

    uint64_t num_reads = 0;
    char **read_ids = slow5_get_rids(sp, &num_reads);
    if(read_ids==NULL){
        fprintf(stderr,"Error in getting list of read IDs\n");
        exit(EXIT_FAILURE);
    }

    for(uint64_t i=0; i<num_reads; i++) {
        printf("%s\n",read_ids[i]);
    }

    slow5_idx_unload(sp);

    slow5_close(sp);

}
```

## SEE ALSO
[slow5_idx_load()](../slow5_idx_load.md)
