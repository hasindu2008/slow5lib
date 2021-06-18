# slow5_get

## NAME

slow5_hdr_get - Gets a header data attribute for a particular read_group.

## SYNOPSYS

`char *slow5_hdr_get(const char *attr, uint32_t read_group, const slow5_hdr_t *header)`

## DESCRIPTION
`slow5_hdr_get()` fetches an attribute value from a SLOW5 file *header* for a specified *attr* in a *read_group* and returns as a *char**.

The argument *header* points to a *slow5_hdr_t* of a *slow5_file_t* opened using `slow5_open()`.

## RETURN VALUE
Upon successful completion, `slow5_hdr_get()` returns a *char* pointer. Otherwise, NULL is returned, e.g., if *attr* doesn't exist or *read_group* is out of range or an input parameter is NULL. 

## NOTES
See also `slow5_get()` and `slow5_get_next()`.

## EXAMPLES

```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example.slow5"

#define TO_PICOAMPS(RAW_VAL,DIGITISATION,OFFSET,RANGE) (((RAW_VAL)+(OFFSET))*((RANGE)/(DIGITISATION)))

int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }
    const slow5_hdr_t* header = sp->header;
    char* read_group_0_run_id_value = slow5_hdr_get("run_id", 0, header);
    if(read_group_0_run_id_value){
        printf("%s\n",read_group_0_run_id_value);
    }

    slow5_close(sp);

}
```
