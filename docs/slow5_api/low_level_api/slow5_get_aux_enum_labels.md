# slow5_get_aux_enum_labels

## NAME

slow5_get_aux_enum_labels - gets the list of labels for an enum data type

## SYNOPSYS

`char **slow5_get_aux_enum_labels(const slow5_hdr_t *header, const char *field, uint8_t *n)`

## DESCRIPTION

`slow5_get_aux_enum_labels()` gets the list of enum labels for a auxiliary field specified by *field* from a SLOW5 file header pointed by *header*. The number of enum labels will be set on the address specified by *n*.


## RETURN VALUE

`slow5_get_aux_enum_labels()` returns the list of enum labels as a *char*** pointer. On error NULL is returned and `slow5_errno` is set to indicate the error.

## ERRORS

In case of an error, `slow5_errno` is set as follows.

* `SLOW5_ERR_NOAUX`
    &nbsp;&nbsp;&nbsp;&nbsp; Auxiliary hash map for the record was not found.
* `SLOW5_ERR_ARG`
    &nbsp;&nbsp;&nbsp;&nbsp; Invalid argument - header or field is NULL.
* `SLOW5_ERR_TYPE`
    &nbsp;&nbsp;&nbsp;&nbsp; Field data type is not an enum type


## NOTES

The returned pointer is from an internal data structure and must NOT be freed by user.
The order of the enum labels corresponds to the enum value, that is, an enum value can be used as the index on the returned pointer to access the corresponding enum label.

## EXAMPLES

```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/adv/example3.blow5"

int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }
    slow5_rec_t *rec = NULL;
    int ret=0;
    ret = slow5_get_next(&rec,sp);
    if(ret<0){
        fprintf(stderr,"Error in slow5_get_next. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }

    uint8_t num_label = 0;
    char **labels = slow5_get_aux_enum_labels(sp->header, "end_reason", &num_label);
    if(labels==NULL){
        fprintf(stderr,"Error in getting list of enum labels\n");
        exit(EXIT_FAILURE);
    }

    uint8_t end_reason = slow5_aux_get_enum(rec,"end_reason",&ret);
    if(ret!=0){
        fprintf(stderr,"Error in getting auxiliary attribute from the file. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }
    if(end_reason != SLOW5_ENUM_NULL){
        printf("end_reason = %s\n", labels[end_reason]);
    } else{
        printf("end_reason is missing for the record\n");
    }

    slow5_rec_free(rec);
    slow5_close(sp);

}
```

## SEE ALSO
[slow5_aux_get_enum()](slow5_aux_get_enum.md)
