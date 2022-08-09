# slow5\_aux\_set

## NAME

slow5\_aux\_set - sets an auxiliary field (a primitive datatype) of a SLOW5 record

## SYNOPSYS

```
int slow5_aux_set(slow5_rec_t *read, const char *field, const void *data, slow5_hdr_t *header)
```

## DESCRIPTION

`slow5_aux_set()` sets the value of an auxiliary field of a primitive datatype (integers, float or double, char) specified by the field name *field* in the slow5 record pointed by *read*. The argument *header* points to a SLOW5 header of type *slow5_hdr_t* that has the corresponding field name along with the datatype already added using `slow5_aux_add()`.

## RETURN VALUE

Upon successful completion, `slow5_aux_set()` returns a non negative integer (>=0). Otherwise, a negative value is returned that indicates the error.


## ERRORS

A negative returned when an error occurs and can be due to following occasions (not an exhaustive list):

- input invalid
- field not found
- type is an array type
- data is invalid

## NOTES

In future `slow5_errno` will be set to indicate the error.

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

    slow5_rec_t *slow5_record = slow5_rec_init();
    if(slow5_record == NULL){
        fprintf(stderr,"Could not allocate space for a slow5 record.");
        exit(EXIT_FAILURE);
    }

    double median_before = 0.1;
    int32_t read_number = 10;
    uint8_t start_mux = 1;
    uint64_t start_time = 100;

    if(slow5_aux_set(slow5_record, "median_before", &median_before, sp->header) < 0){
        fprintf(stderr,"Error setting median_before auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    if(slow5_aux_set(slow5_record, "read_number", &read_number, sp->header) < 0){
        fprintf(stderr,"Error setting read_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    if(slow5_aux_set(slow5_record, "start_mux", &start_mux, sp->header) < 0){
        fprintf(stderr,"Error setting start_mux auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    if(slow5_aux_set(slow5_record, "start_time", &start_time, sp->header) < 0){
        fprintf(stderr,"Error setting start_time auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    //....

}
```

## SEE ALSO
[`slow5_aux_set_string()`](slow5_aux_set_string.md).
