# slow5_open

## NAME
slow5_open - opens a SLOW5 file

## SYNOPSYS
`slow5_file_t *slow5_open(const char *pathname, const char *mode)`

## DESCRIPTION

The `slow_open()` function opens a SLOW5 file (ASCII or binary) pointed by the argument *pathname*, parses and populates the SLOW5 header. `slow_open()` determines if the file is SLOW5 ASCII or SLOW5 Binary from extension of the argument *pathname* (*.slow5* for SLOW5 ASCII and *.blow5* for SLOW5 binary).

Currently, the argument *mode* points to a string which can be one of the following:

* `r`   Open a SLOW5 file for reading.
* `w`   Open a SLOW5 file for writing. Creates a new file if the file does not exit. An existing file is truncated to zero.
* `a`   Open a SLOW5 file for appending (writing at end of file). The file must already exist. If it does not exist, `slow_open()` will fail. If *pathname* is a BLOW5 file, the existing SLOW5 EOF marker is overwritten.

An open slow5 file should be closed at the end using `slow5_close()` function.


## RETURN VALUE

Upon successful completion, `slow_open()` returns a *slow5_file_t* pointer. Otherwise, NULL is returned and `slow5_errno` is set to indicate the error.

## ERRORS

* `SLOW5_ERR_ARG`
  &nbsp;&nbsp;&nbsp;&nbsp; Invalid argument - pathname or mode provided was NULL.
* `SLOW5_ERR_IO`
  &nbsp;&nbsp;&nbsp;&nbsp; File I/O error, for instance, `fopen`, `ftello` or `fileno` failed.
* `SLOW5_ERR_UNK`
  &nbsp;&nbsp;&nbsp;&nbsp; Wrong file extension, that is neither *.slow5* nor *.blow5*
* `SLOW5_ERR_HDRPARSE`
  &nbsp;&nbsp;&nbsp;&nbsp; Parsing and populating the header failed.
* `SLOW5_ERR_MEM`
  &nbsp;&nbsp;&nbsp;&nbsp; Memory allocation failed.
* `SLOW5_ERR_TRUNC`
  &nbsp;&nbsp;&nbsp;&nbsp; End of file (EOF) reached prematurely.
* `SLOW5_ERR_MAGIC`
  &nbsp;&nbsp;&nbsp;&nbsp; Invalid magic number.
* `SLOW5_ERR_VERSION`
  &nbsp;&nbsp;&nbsp;&nbsp; File version is incompatible with this library version.
* `SLOW5_ERR_PRESS`
  &nbsp;&nbsp;&nbsp;&nbsp; Initialisation of compression/decompression buffers failed.
* `SLOW5_ERR_OTH`
  &nbsp;&nbsp;&nbsp;&nbsp; Other error.


## NOTES
Internally uses `fopen()`. If *mode* is `r`, the stream is positioned at the beginning of the data records when `slow_open()` returns. If mode is `w`, the stream is positioned at the end of the file. If mode is `a`, the stream is positioned at the end of the file for .slow5; and 5 bytes before the end of the file for .blow5 such that the existing SLOW5 EOF marker is overwritten.


## EXAMPLES
```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example.slow5"

int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }

    //...

    slow5_close(sp);

}
```

## SEE ALSO

[slow5_close()](slow5_close.md)
