# slow5\_aux\_get\_enum

## NAME

slow5\_aux\_get\_enum - fetches an auxiliary field of type enum from a SLOW5 record

## SYNOPSYS

```
uint8_t slow5_aux_get_enum(const slow5_rec_t *read, const char *field, int *err)
```


## DESCRIPTION

`slow5_aux_get_enum()` fetches the value of an auxiliary field of a enum datatype specified by the field name *field* from the slow5 record pointed by *read*.

The argument *err* is an address of an integer which will be set inside the function call to indicate an error. Unless *err* is NULL, `slow5_aux_get_enum()` sets a non zero error code in **err* in case of failure.

## RETURN VALUE

Upon successful completion, `slow5_aux_get_enum()` returns the requested enum field value as a uint8_t. Otherwise, the SLOW5 missing value representation for the enum datatype is returned (`SLOW5_ENUM_NULL`) and `slow5_errno` is set to indicate the error. See notes below.


## ERRORS

In case of an error, `slow5_errno` or the non zero error code is set in *err* (unless *err* is NULL) are as follows.

* `SLOW5_ERR_NOFLD`
    &nbsp;&nbsp;&nbsp;&nbsp; The requested field was not found.
* `SLOW5_ERR_NOAUX`
    &nbsp;&nbsp;&nbsp;&nbsp; Auxiliary hash map for the record was not found (see notes below).
* `SLOW5_ERR_ARG`
    &nbsp;&nbsp;&nbsp;&nbsp; Invalid argument - read or field is NULL.
* `SLOW5_ERR_TYPE`
    &nbsp;&nbsp;&nbsp;&nbsp; Type conversion was not possible - an array data type field cannot be converted to a primitive type.


## NOTES

More error codes may be introduced in future. If the field value has been marked missing for an individual SLOW5 record ("." in SLOW5 ASCII and as `SLOW5_ENUM_NULL` in BLOW5), `SLOW5_ENUM_NULL` is returned and no error code is set (considered successful as the SLOW5 missing value representation is actually present in the file). This is typically the case when the field name is present in the SLOW5 header, but the field value for the particular record is missing. If the requested field is completely not present (not in the SLOW5 header as well), this is considered an error and `SLOW5_ERR_NOAUX` code is set.

The enum value returned by `slow5_aux_get_enum()` can be used to obtain the corresponding enum label by using using as the index for the list returned by `slow5_get_aux_enum_labels()`.

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
[`slow5_get_aux_enum_labels()`](slow5_get_aux_enum_labels.md).
