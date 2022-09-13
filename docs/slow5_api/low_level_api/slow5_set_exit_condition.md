# slow5_set_exit_condition

## NAME

slow5_set_exit_condition - sets if slow5lib should exit the programme on error

## SYNOPSYS

`void slow5_set_exit_condition(enum slow5_exit_condition_opt exit_condition)`

## DESCRIPTION

`slow5_set_exit_condition()` sets the slow5lib to exit the programme on a circumstance as specified by *exit_condition*. *exit_condition* can be one of the following:

- `SLOW5_EXIT_OFF`: Do not exit the programme even if an error occur. The user should have done error handling for each function call which is typical library behaviour in C (default).
- `SLOW5_EXIT_ON_ERR`: Exit the programme if an error occurs (useful for debugging)
- `SLOW5_EXIT_ON_WARN`: Exit the programme if a warning occurs. This is a bit silly, but in case someone wants.

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

	slow5_set_exit_condition(SLOW5_EXIT_ON_ERR);

	//... do the rest

}
```

## SEE ALSO
[slow5_set_log_level()](slow5_set_log_level.md)
