# slow5_hdr_set

## NAME

slow5_hdr_set -  sets a header data attribute for a particular read_group

## SYNOPSYS

`int slow5_hdr_set(const char *attr, const char *value, uint32_t read_group, slow5_hdr_t *header)`

## DESCRIPTION
`slow5_hdr_set()` sets the attribute value *value* for specified attribute name *attr* of a specified read group *read_group* in a SLOW5 file header pointed by *header*.

The argument *header* points to a SLOW5 header of type *slow5_hdr_t* and typically this is the *s5p->header* member inside the *slow5_file_t \*s5p* returned by  `slow5_open()`.

## RETURN VALUE
Upon successful completion, `slow5_hdr_set()` returns a non negative integer (>=0). Otherwise, a negative value is returned that indicates the error.

## ERRORS

A negative value is returned when an error occurs and can be due to following occasions (not an exhaustive list):

- attribute name doesn't exist
- read group is out of range
- an input parameter is NULL

## NOTES

In the future `slow5_errno` will be set to indicate the error.

## EXAMPLES

```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "test.slow5"

int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH, "w");
    if(sp==NULL){
        fprintf(stderr,"Error opening file!\n");
        exit(EXIT_FAILURE);
    }
    slow5_hdr_t* header = sp->header;

    //add a header group attribute called run_id
    if (slow5_hdr_add("run_id", header) != 0){
        fprintf(stderr,"Error adding run_id attribute\n");
        exit(EXIT_FAILURE);
    }

    //set the run_id attribute to "run_0" for read group 0
    if (slow5_hdr_set("run_id", "run_0", 0, header) != 0){
        fprintf(stderr,"Error setting run_id attribute in read group 0\n");
        exit(EXIT_FAILURE);
    }

    //....

}
```
