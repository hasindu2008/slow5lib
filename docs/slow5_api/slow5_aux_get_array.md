# slow5_aux_get_\<array_datatype\>

## NAME

slow5_aux_get_\<array_datatype\> - fetches an auxiliary field (an array datatype) of a SLOW5 record

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
`slow5_aux_get_\<array_datatype\>()` fetches the value of an auxiliary field of an array datatype specified by the filed name *field* from the slow5 record pointed by *read*. User has to choose the appropriate function to match the datatype of the value returned.

The argument *len* is an address of an integer which will be set inside the function call to indicate an error.

The argument *err* is an address of a *uint64_t* usigned integer which will be set inside the function to indicate the length of the returned array.

`slow5_aux_get_\<array_datatype\>()` sets a non zero error code in *err* in case of failure.

## RETURN VALUE

Upon successful completion, `slow5_aux_get_\<array_datatype\>()` returns a pointer to an array.

## ERRORS
A non zero error code is set in *err*.

## NOTES

The returned pointer is from an internal data structure and the user must NOT free this.

`slow5_aux_get_string()` is used to fetch an array of chars.



Error codes are not finalised and subject to change.


## EXAMPLES
TODO


## SEE ALSO
