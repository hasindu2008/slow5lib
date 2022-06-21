# slow5_close

## NAME
slow5_close - closes an open slow5 file and free the associated memory

## SYNOPSYS
`int slow5_close(slow5_file_t *s5p)`

## DESCRIPTION
`slow5_close()` closes a slow5 file and frees the associated *slow5_file_t* structure from memory.
The argument *s5p* is the *slow5_file_t* pointer returned by a previous `slow5_open()` call. If a BLOW5 file had been opened for writing or appending, the SLOW5 EOF marker is appended to the end of the file.

The behaviour of `slow5_close()` is undefined if the parameter is an illegal pointer, or if `slow5_close()` has been previous invoked on the pointer.

If a slow5 index has been associated with the *s5p* structure (using a previous `slow5_idx_load()` invocation), `slow5_idx_unload()` must be invoked before calling `slow5_close()`.



## RETURN VALUE

Upon successful completion, `slow5_close()` returns 0. Otherwise, a negative value (EOF) is returned and `slow5_errno` is set to indicate the error.

## ERRORS

* `SLOW5_ERR_IO`
  &nbsp;&nbsp;&nbsp;&nbsp;File I/O error, for instance, `fclose` failed.

## NOTES

Internally uses `fclose()` to close the file stream and calls `free()` on allocated memory.

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

[slow5_open()](slow5_open.md)
