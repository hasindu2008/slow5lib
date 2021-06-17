# slow5lib

## NAME
slow5_get_next - Get the read entry under the current file pointer of a slow5 file.

## SYNOPSYS
`int slow5_get_next(slow5_rec_t **read, slow5_file_t *s5p)`

## DESCRIPTION
`slow5_get_next()` fetches a record from a SLOW5 file *s5p* at the current file pointer into a *slow5_rec_t* and stores the address of the *slow5_rec_t* in **read*.

If **read* is set to NULL before the call, then `slow5_get()` will allocate a *slow5_rec_t* for storing the record.
This *slow5_rec_t* should be freed by the user program using `slow5_rec_free()`.
Alternatively, before calling `slow5_get()`, **read* can contain a pointer to an allocated *slow5_rec_t* from a previous `slow5_get()` call.
If the allocated *slow5_rec_t* is not large enough to hold the record, `slow5_get()` will resize it internally.

The argument *s5p* points to a *slow5_file_t* opened using `slow5_open()`.

## RETURN VALUE
Upon successful completion, `slow5_get_next()` returns 0. Otherwise, a negative value is returned that indicates the error.

## ERRORS
A negative return value indicates an error as follows.

* `-1`
    read_id, read or s5p is NULL
* `-2`
    reading error when reading the slow5 file
* `-3`
    parsing error

## NOTES
 As opposed to `slow5_get()` which requires the SLOW index to be pre-loaded to *s5p* using `slow5_idx_load()`, `slow5_get_next` does not require an index.

Also see `slow5_get()`

## EXAMPLES

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
    slow5_rec_t *rec = NULL;
    int ret=0;

    ret = slow5_get_next(&rec, sp);
    if(ret < 0){
        fprintf(stderr,"Error in locating read\n");
    }
    else{
        printf("%s\t",rec->read_id);
        uint64_t len_raw_signal = rec->len_raw_signal;
        for(uint64_t i=0;i<len_raw_signal;i++){
            double pA = TO_PICOAMPS(rec->raw_signal[i],rec->digitisation,rec->offset,rec->range);
            printf("%f ",pA);
        }
        printf("\n");
    }

    slow5_close(sp);

}
```
