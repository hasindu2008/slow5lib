# slow5_set_log_level

## NAME

slow5_set_log_level - sets the level log message verbosity

## SYNOPSYS

`void slow5_set_log_level(enum slow5_log_level_opt log_level)`

## DESCRIPTION

`slow5_set_log_level()` sets the level of verbosity in the slow5lib log messages as specified by *log_level*. *log_level* can be one of the following:

- `SLOW5_LOG_OFF`:  nothing at all
- `SLOW5_LOG_ERR`:  error messages
- `SLOW5_LOG_WARN`: warning and error messages
- `SLOW5_LOG_INFO`: information, warning and error messages (default)
- `SLOW5_LOG_VERB`: verbose, information, warning and error messages
- `SLOW5_LOG_DBUG`: debugging, verbose, information, warning and error messages

If `slow5_set_log_level` is not invoked,  default value is `SLOW5_LOG_INFO`.

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

	slow5_set_log_level(SLOW5_LOG_DBUG);

	//... do the rest

}
```

## SEE ALSO
[slow5_set_exit_condition()](slow5_set_exit_condition.md).
