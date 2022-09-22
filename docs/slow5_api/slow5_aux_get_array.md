# slow5\_aux\_get\_\<array_datatype\>

## NAME

slow5\_aux\_get\_\<array_datatype\> - fetches an auxiliary field (an array datatype) of a SLOW5 record

## SYNOPSYS

```
int8_t *slow5_aux_get_int8_array(const slow5_rec_t *read, const char *field, uint64_t *len, int *err)
int16_t *slow5_aux_get_int16_array(const slow5_rec_t *read, const char *field, uint64_t *len, int *err)
int32_t *slow5_aux_get_int32_array(const slow5_rec_t *read, const char *field, uint64_t *len, int *err)
int64_t *slow5_aux_get_int64_array(const slow5_rec_t *read, const char *field, uint64_t *len, int *err)

uint8_t *slow5_aux_get_uint8_array(const slow5_rec_t *read, const char *field, uint64_t *len, int *err)
uint16_t *slow5_aux_get_uint16_array(const slow5_rec_t *read, const char *field, uint64_t *len, int *err)
uint32_t *slow5_aux_get_uint32_array(const slow5_rec_t *read, const char *field, uint64_t *len, int *err)
uint64_t *slow5_aux_get_uint64_array(const slow5_rec_t *read, const char *field, uint64_t *len, int *err)

float *slow5_aux_get_float_array(const slow5_rec_t *read, const char *field, uint64_t *len, int *err)
double *slow5_aux_get_double_array(const slow5_rec_t *read, const char *field, uint64_t *len, int *err)

char *slow5_aux_get_string(const slow5_rec_t *read, const char *field, uint64_t *len, int *err)
```


## DESCRIPTION
`slow5_aux_get_<array_datatype>()` fetches the value of an auxiliary field of an array datatype specified by the filed name *field* from the slow5 record pointed by *read*.  User has to choose the appropriate function so that the returned datatype matches the correct datatype of the stored field, because the data stored in memory as a series of bytes will be reinterpreted as the requested type.

The argument *len* is an address of a *uint64_t* unsigned integer which will be set inside the function to indicate the length of the returned array.

The argument *err* is an address of an integer which will be set inside the function call to indicate an error. Unless *err* is NULL, `slow5_aux_get_<array_datatype>()`sets a non zero error code in **err* in case of failure.


## RETURN VALUE

Upon successful completion, `slow5_aux_get_<array_datatype>()` returns a pointer to an array.  Otherwise, NULL is returned and `slow5_errno` is set to indicate the error.

## ERRORS

In case of an error, `slow5_errno` or the non zero error code is set in *err* (unless *err* is NULL) are as follows.

* `SLOW5_ERR_NOFLD`
    &nbsp;&nbsp;&nbsp;&nbsp; The requested field was not found.
* `SLOW5_ERR_NOAUX`
    &nbsp;&nbsp;&nbsp;&nbsp; Auxiliary hash map for the record was not found.
* `SLOW5_ERR_ARG`
    &nbsp;&nbsp;&nbsp;&nbsp; Invalid argument - read or field is NULL
* `SLOW5_ERR_TYPE`
    &nbsp;&nbsp;&nbsp;&nbsp; Type conversion was not possible - a primitives data type field cannot be converted to an array type.

## NOTES

The returned pointer is from an internal data structure and the user must NOT free this.

`slow5_aux_get_string()` is used to fetch an array of chars.

More error may be introduced in future. If the field value array has been marked missing for an individual SLOW5 record (represented by “.” in SLOW5 ASCII and in BLOW by storing 0 as the length of the array), NULL is returned with *len* argument being set to 0 and NO error code is set (considered successful as the SLOW5 missing value representation is actually present in the file). This is typically the case when the field name is present in the SLOW5 header, but the field value for the particular record is missing. If the requested field is completely not present (not in the SLOW5 header as well), this is considered an error and `SLOW5_ERR_NOAUX` code is set.


## EXAMPLES
```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example2.slow5"

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

    //------------------------------------------------------------------------
    //              get auxiliary values with array datatype
    //------------------------------------------------------------------------

    uint64_t len;
    char* channel_number = slow5_aux_get_string(rec, "channel_number", &len, &ret);
    if(ret!=0){
        fprintf(stderr,"Error in getting auxiliary attribute from the file. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }
    if (channel_number != NULL){
        printf("channel_number = %s\n", channel_number);
    } else{
        printf("channel_number is missing for the record\n");
    }

    slow5_rec_free(rec);
    slow5_close(sp);

}
```

## SEE ALSO
[`slow5_aux_get_<primitive_datatype>()`](slow5_aux_get.md).
