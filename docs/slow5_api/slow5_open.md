# slow5_open

## NAME
slow5_open - Opens a SLOW5 file.

## SYNOPSYS
```
#include <slow5/slow5.h>
slow5_file_t *slow5_open(const char *pathname, const char *mode)`
```

## DESCRIPTION

The `slow_open()` function opens a SLOW5 file (ASCII and binary) pointed by the argument *pathname*, parses and populates the SLOW5 header. `slow_open()` determines if the file is SLOW5 ASCII or SLOW5 Binary from extension of the argument *pathname* (*.slow5* for SLOW5 ASCII and *.blow5* for SLOW5 binary).

The argument *mode* points to a string beginning with the following sequence. Additional modes will be introduced later.
- *r* Open SLOW5 file for reading

`slow_open()`

An open slow5 file should be closed at the end using `slow5_close()` function.


## RETURN VALUE
Upon successful completion, `slow_open()` returns a *slow5_file_t* pointer. Otherwise, NULL is returned.

## ERRORS

`slow_open()` fails and returns NULL in the following occasions:
- pathname or mode is NULL
- pathname is invalid (i.e., internally `fopen()` failed on the pathname)
- wrong file extension that is neither .slow5 or .blow5
- parsing and populating the header failed
- `fileno()` failed internally

## NOTES
Internally uses `fopen()`. The stream is positioned at the beginning of the data records.
