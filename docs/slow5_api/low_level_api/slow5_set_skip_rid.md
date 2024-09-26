# slow5_set_skip_rid

## NAME

slow5_set_skip_rid - sets that a requested read missing is not to be treated as an error

## SYNOPSYS

`void slow5_set_skip_rid()`

## DESCRIPTION

`slow5_set_skip_rid()` sets that a requested read missing is not to be treated as an error. The default of behaviour of functions such as `slow5_get` that loads a requested read from a SLOW5 file is to print an error message and exit the programme if that read isn't found. This function can be used to disable this in legitimate cases where we expect some reads would be missing. Note that the `slow5_get` will still return a value indicating the error which can be used by the programmer to handle the case as they wish.

## RETURN VALUE

None.

## ERRORS

None.


## NOTES

Internally sets global variable. Intended to be set as the beginning of the programme.

## EXAMPLES

```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

int main(){

	slow5_set_skip_rid(SLOW5_LOG_DBUG);

	//... do the rest

}
```

## SEE ALSO
[slow5_set_log_level()](slow5_set_log_level.md)
[slow5_set_exit_condition()](slow5_set_exit_condition.md).
