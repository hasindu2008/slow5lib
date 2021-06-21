# slow5lib

## NAME
slow5_close - Closes a slow5 file and free its memory.

## SYNOPSYS
`int slow5_close(slow5_file_t *s5p)`

## DESCRIPTION
This is a wrapper function around the standard C library function `fclose()`.
`slow5_close()` safely closes attributes defined in *slow5_file_t*

A SLOW5 file *s5p* that was opened using `slow5_open()` or `slow5_open_with()` should be passed to `slow5_close()` to close the file .

## RETURN VALUE
Same as the behavior observed in the standard C library function `fclose()`.

## NOTES



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

[`slow5_open()`](slow5_open.md).
