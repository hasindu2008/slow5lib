# slow5_get_hdr_keys

## NAME

slow5_get_hdr_keys - gets the list of data header attribute keys

## SYNOPSYS

`const char **slow5_get_hdr_keys(const slow5_hdr_t *header, uint64_t *len)`

## DESCRIPTION

`slow5_get_hdr_keys()` gets the list of data header attribute keys from a SLOW5 file header pointed by *header*. The number of attribute keys will be set on the address specified by *len*.


## RETURN VALUE

`slow5_get_hdr_keys()` returns the list of attribute keys as a *char*** pointer. If there are no header attributes, NULL is returned. The retuned pointer must be freed by user.


## ERRORS

TODO


## NOTES

Only the returned *char*** pointer must be freed and not the the actual key labels (char *) which are from an internal data structure.

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

    uint64_t num_keys = 0;
    const char **keys = slow5_get_hdr_keys(sp->header, &num_keys);
    if(keys==NULL){
        fprintf(stderr,"Error in getting header keys\n");
        exit(EXIT_FAILURE);
    }

    for(int i=0; i<num_keys; i++) {
        printf("%s\n",keys[i]);
    }

	free(keys);

    slow5_close(sp);

}
```

## SEE ALSO
[slow5_yy()](slow5_yy.md), [slow5_yy()](slow5_yy.md)
