# slow5_get

## NAME

slow5_aux_get_<array_datatype> - Get an auxiliary field (an array field) in a SLOW5 record and returns the pointer to an array of the corresponding data type.

## SYNOPSYS

`DATA_TYPE* slow5_aux_get_DATA_TYPE_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err)`
`int8_t *slow5_aux_get_int8_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err)`

`int16_t *slow5_aux_get_int16_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err)`

`int32_t *slow5_aux_get_int32_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err)`

`int64_t *slow5_aux_get_int64_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err)`

`uint8_t *slow5_aux_get_uint8_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err)`

`uint16_t *slow5_aux_get_uint16_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err)`

`uint32_t *slow5_aux_get_uint32_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err)`

`uint64_t *slow5_aux_get_uint64_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err)`

`float *slow5_aux_get_float_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err)`

`double *slow5_aux_get_double_array(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err)`

`char *slow5_aux_get_string(const slow5_rec_t *read, const char *attr, uint64_t *len, int *err)`


## DESCRIPTION
`slow5_aux_get_<array_datatype>()` fetches the value of an auxiliary field *attr* from the slow5 record *read* as an array of type DATA_TYPE of length *len*. User has to choose the appropriate function to match the datatype of the value returned.

`slow5_aux_get_<array_datatype>()` sets a non zero error code in *err* in case of failure.

## RETURN VALUE

Upon successful completion, `slow5_aux_get_<array_datatype>()` returns a value.

## ERRORS
A non zero error code is set in *err*.

## NOTES
`slow5_aux_get_string()` is used to fetch an array of chars.

Error codes are not finalised and subject to change.

See also `slow5_get_aux_names()` and `slow5_get_aux_types()`.

## EXAMPLES
TODO
