# slow5_hdr_get

## NAME

slow5_hdr_get -  fetches a header data attribute from a SLOW5 header

## SYNOPSYS

`char *slow5_hdr_get(const char *attr, uint32_t read_group, const slow5_hdr_t *header)`

## DESCRIPTION
`slow5_hdr_get()` fetches the attribute value for specified attribute name *attr* of a specified read group *read_group* from a SLOW5 file header pointed by *header* and returns as a *char** pointers.

The argument *header* points to a SLOW5 header of type *slow5_hdr_t* and typically this is the *s5p->header* member inside the *slow5_file_t *s5p* returned by  `slow5_open()`.


## RETURN VALUE
Upon successful completion, `slow5_hdr_get()` returns the attribute value as *char** pointer. Otherwise, NULL is returned.

## ERRORS

NULL is returned when an error occurs and can be due to following occasions (not an exhaustive list):

- *attr* doesn't exist
- *read_group* is out of range
- input parameter is NULL

## NOTES

The returned pointer is from an internal data structure and the user must NOT free this.
In the future a error number will be set to indicate the error.

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
    const slow5_hdr_t* header = sp->header;
    char* read_group_0_run_id_value = slow5_hdr_get("run_id", 0, header);
    if(read_group_0_run_id_value){
        printf("%s\n",read_group_0_run_id_value);
    }

    slow5_close(sp);

}
```
