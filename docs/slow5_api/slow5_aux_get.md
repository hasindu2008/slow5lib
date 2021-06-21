# slow5_aux_get_<primitive_datatype>

## NAME

slow5_aux_get_<primitive_datatype> - Get an auxiliary field in a SLOW5 record as specified in DATA_TYPE.

## SYNOPSYS

`DATA_TYPE slow5_aux_get_DATA_TYPE(const slow5_rec_t *read, const char *attr, int *err)`

`int8_t slow5_aux_get_int8(const slow5_rec_t *read, const char *attr, int *err)`

`int16_t slow5_aux_get_int16(const slow5_rec_t *read, const char *attr, int *err)`

`int32_t slow5_aux_get_int32(const slow5_rec_t *read, const char *attr, int *err)`

`int64_t slow5_aux_get_int64(const slow5_rec_t *read, const char *attr, int *err)`

`uint8_t slow5_aux_get_uint8(const slow5_rec_t *read, const char *attr, int *err)`

`uint16_t slow5_aux_get_uint16(const slow5_rec_t *read, const char *attr, int *err)`

`uint32_t slow5_aux_get_uint32(const slow5_rec_t *read, const char *attr, int *err)`

`uint64_t slow5_aux_get_uint64(const slow5_rec_t *read, const char *attr, int *err)`

`float slow5_aux_get_float(const slow5_rec_t *read, const char *attr, int *err)`

`double slow5_aux_get_double(const slow5_rec_t *read, const char *attr, int *err)`

`char slow5_aux_get_char(const slow5_rec_t *read, const char *attr, int *err)`


## DESCRIPTION

`slow5_aux_get_<primitive_datatype>()` fetches the value of an auxiliary field *attr* from the slow5 record *read*. User has to choose the appropriate function to match the datatype of the value returned.

`slow5_aux_get_<primitive_datatype>()` sets a non zero error code in *err* in case of failure.

## RETURN VALUE

Upon successful completion, `slow5_aux_get_<primitive_datatype>()` returns a value.

## ERRORS
A non zero error code is set in *err*.

## NOTES

Error codes are not finalised and subject to change.

See also `slow5_get_aux_names()` and `slow5_get_aux_types()`.

## EXAMPLES
TODO
