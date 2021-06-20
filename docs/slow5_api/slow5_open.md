# slow5_open

## NAME
slow5_open - Opens a SLOW5 file.

## SYNOPSYS
`slow5_file_t *slow5_open(const char *pathname, const char *mode)`

## DESCRIPTION

The `slow_open()` function opens a SLOW5 file (ASCII and binary) pointed by the argument *pathname*, parses and populates the SLOW5 header. `slow_open()` determines if the file is SLOW5 ASCII or SLOW5 Binary from extension of the argument *pathname* (*.slow5* for SLOW5 ASCII and *.blow5* for SLOW5 binary).

Currently, the argument *mode* points to a string "r" for opening a SLOW5 file for reading. Additional modes for writing will be introduced later.

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


```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example.slow5"

#define TO_PICOAMPS(RAW_VAL,DIGITISATION,OFFSET,RANGE) (((RAW_VAL)+(OFFSET))*((RANGE)/(DIGITISATION)))

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
