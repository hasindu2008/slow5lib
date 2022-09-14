# slow5_aux_add

## NAME

slow5_aux_add -  adds an auxiliary field to a SLOW5 header

## SYNOPSYS

`int slow5_aux_add(const char *field, enum slow5_aux_type type, slow5_hdr_t *header)`

## DESCRIPTION
`slow5_aux_add()` adds an auxiliary field named *field* of the datatype *type* to a SLOW5 file header pointed by *header*.

The argument *header* points to a SLOW5 header of type *slow5_hdr_t* and typically this is the *s5p->header* member inside the *slow5_file_t \*s5p* returned by  `slow5_open()`.

*type* can be a primitive datatype or an array data type.
Available primitive data types are:
- SLOW5_INT8_T
- SLOW5_INT16_T
- SLOW5_INT32_T
- SLOW5_INT64_T
- SLOW5_UINT8_T
- SLOW5_UINT16_T
- SLOW5_UINT32_T
- SLOW5_UINT64_T
- SLOW5_FLOAT
- SLOW5_DOUBLE
- SLOW5_CHAR

Available array data types are:
- SLOW5_INT8_T_ARRAY
- SLOW5_INT16_T_ARRAY
- SLOW5_INT32_T_ARRAY
- SLOW5_INT64_T_ARRAY
- SLOW5_UINT8_T_ARRAY
- SLOW5_UINT16_T_ARRAY
- SLOW5_UINT32_T_ARRAY
- SLOW5_UINT64_T_ARRAY
- SLOW5_FLOAT_ARRAY
- SLOW5_DOUBLE_ARRAY
- SLOW5_STRING

## RETURN VALUE
Upon successful completion, `slow5_aux_add()` returns a non negative integer (>=0). Otherwise, a negative value is returned.

## ERRORS

A negative value is returned when an error occurs and can be due to following occasions (not an exhaustive list):

- input parameter is NULL
- unsuitable datatype
- other error

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

    //add auxilliary field: channel number
    if (slow5_aux_add("channel_number", SLOW5_STRING, sp->header) < 0){
        fprintf(stderr,"Error adding channel_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    //add auxilliary field: median_before
    if (slow5_aux_add("median_before", SLOW5_DOUBLE, sp->header) < 0) {
        fprintf(stderr,"Error adding median_before auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    //add auxilliary field: read_number
    if(slow5_aux_add("read_number", SLOW5_INT32_T, sp->header) < 0){
        fprintf(stderr,"Error adding read_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    //add auxilliary field: start_mux
    if(slow5_aux_add("start_mux", SLOW5_UINT8_T, sp->header) < 0){
        fprintf(stderr,"Error adding start_mux auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    //add auxilliary field: start_time
    if(slow5_aux_add("start_time", SLOW5_UINT64_T, sp->header) < 0){
        fprintf(stderr,"Error adding start_time auxilliary field\n");
        exit(EXIT_FAILURE);
    }


    //....

}
```
