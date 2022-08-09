# slow5\_aux\_set\_string

## NAME

slow5\_aux\_set\_string - sets a string auxiliary field (char* array) of a SLOW5 record

## SYNOPSYS

```
int slow5_aux_set_string(slow5_rec_t *read, const char *field, const char *data, slow5_hdr_t *header);
```

## DESCRIPTION

`slow5_aux_set_string()` sets the value of an auxiliary field of string datatype (char* array) specified by the field name *field* in the slow5 record pointed by *read*. The argument *header* points to a SLOW5 header of type *slow5_hdr_t* that has the corresponding field name along with the string datatype already added using `slow5_aux_add()`.

## RETURN VALUE

Upon successful completion, `slow5_aux_set_string()` returns a non negative integer (>=0). Otherwise, a negative value is returned that indicates the error.


## ERRORS

A negative returned when an error occurs and can be due to following occasions (not an exhaustive list):

- input invalid
- field not found
- wrong type
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

    //add auxilliary field: channel number
    if (slow5_aux_add("channel_number", SLOW5_STRING, sp->header) < 0){
        fprintf(stderr,"Error adding channel_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    slow5_rec_t *slow5_record = slow5_rec_init();
    if(slow5_record == NULL){
        fprintf(stderr,"Could not allocate space for a slow5 record.");
        exit(EXIT_FAILURE);
    }

    char *channel_number = "0";

    if(slow5_aux_set_string(slow5_record, "channel_number", channel_number, sp->header) < 0){
        fprintf(stderr,"Error setting channel_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    //....

}
```

## SEE ALSO
[`slow5_aux_set_string()`](slow5_aux_set_string.md).
