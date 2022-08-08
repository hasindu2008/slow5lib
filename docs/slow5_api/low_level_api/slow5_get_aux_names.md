# slow5_get_aux_names

## NAME

slow5_get_aux_names - gets the pointer to the list of auxiliary field names

## SYNOPSYS

`char **slow5_get_aux_names(const slow5_hdr_t *header, uint64_t *len)`

## DESCRIPTION

`slow5_get_aux_names()` gets the list of auxiliary field names from a SLOW5 file header pointed by *header*. The number of fields will be set on the address specified by *len*.


## RETURN VALUE

Upon successful completion, `slow5_get_aux_names()` returns the list of auxiliary fields as a *char*** pointer. If there are no auxiliary fields, NULL is returned.


## ERRORS



## NOTES

`slow5_get_aux_names()` returns the pointer to an internal list. Thus, the retuned pointer must NOT be freed by user.

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

    uint64_t num_aux = 0;
    char **aux = slow5_get_aux_names(sp->header, &num_aux);
    if(aux==NULL){
        fprintf(stderr,"Error in getting list of aux field names\n");
        exit(EXIT_FAILURE);
    }

    for(uint64_t i=0; i<num_aux; i++) {
        printf("%s\n",aux[i]);
    }

    slow5_close(sp);

}
```

## SEE ALSO
[slow5_yy()](slow5_yy.md), [slow5_yy()](slow5_yy.md)
