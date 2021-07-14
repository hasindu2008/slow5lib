# slow5_aux_get_\<primitive_datatype\>

## NAME

slow5_aux_get_\<primitive_datatype\> - fetches an auxiliary field (a primitive datatype) from a SLOW5 record

## SYNOPSYS

```
int8_t slow5_aux_get_int8(const slow5_rec_t *read, const char *field, int *err)
int16_t slow5_aux_get_int16(const slow5_rec_t *read, const char *field, int *err)
int32_t slow5_aux_get_int32(const slow5_rec_t *read, const char *field, int *err)
int64_t slow5_aux_get_int64(const slow5_rec_t *read, const char *field, int *err)

uint8_t slow5_aux_get_uint8(const slow5_rec_t *read, const char *field, int *err)
uint16_t slow5_aux_get_uint16(const slow5_rec_t *read, const char *field, int *err)
uint32_t slow5_aux_get_uint32(const slow5_rec_t *read, const char *field, int *err)
uint64_t slow5_aux_get_uint64(const slow5_rec_t *read, const char *field, int *err)

float slow5_aux_get_float(const slow5_rec_t *read, const char *field, int *err)
double slow5_aux_get_double(const slow5_rec_t *read, const char *field, int *err)

char slow5_aux_get_char(const slow5_rec_t *read, const char *field, int *err)
```


## DESCRIPTION

`slow5_aux_get_<primitive_datatype>()` fetches the value of an auxiliary field of a primitive datatype specified by the filed name *field* from the slow5 record pointed by *read*. User has to choose the appropriate function to match the datatype of the value returned.

The argument *err* is an address of an integer which will be set inside the function call to indicate an error.

`slow5_aux_get_<primitive_datatype>()` sets a non zero error code in **err* in case of failure.

## RETURN VALUE

Upon successful completion, `slow5_aux_get_<primitive_datatype>()` returns the requested field value.

## ERRORS
A non zero error code is set in *err*.

## NOTES
Error codes are not finalised and subject to change.

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
    //              get auxiliary values with primitive datatype
    //------------------------------------------------------------------------
    ret=0;
    double median_before = slow5_aux_get_double(rec,"median_before",&ret);
    if(ret!=0){
        fprintf(stderr,"Error in getting auxiliary attribute from the file. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"median_before = %f\n", median_before);

    uint64_t start_time = slow5_aux_get_uint64(rec, "start_time", &ret);
    if(ret!=0){
        fprintf(stderr,"Error in getting auxiliary attribute from the file. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"start_time = %u\n", start_time);

    slow5_rec_free(rec);
    slow5_close(sp);

}
```

## SEE ALSO
[`slow5_aux_get_<array_datatype>()`](slow5_aux_get_array.md).