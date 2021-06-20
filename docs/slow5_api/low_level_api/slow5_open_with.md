# slow5lib

## NAME
slow5_open_with - Opens a slow5 file of a specific format with a mode given it's pathname.

## SYNOPSYS
`slow5_file_t *slow5_open_with(const char *pathname, const char *mode, enum slow5_fmt format)`

## DESCRIPTION
This function at the moment should only be used to open a file for reading. The user is expected to give `r` or `rb` as the *mode* for ASCII and binary respectively along with the correct *format*.

An open slow5 file should be closed using `slow5_close()`

## RETURN VALUE
Upon successful completion, `slow5_open_with()` returns a *slow5_file_t* pointer. Otherwise, NULL is returned, e.g., if *pathname* or *mode* is NULL, or if the *format* specified doesn't match the file format.


## NOTES
This function at the moment should only be used to open a file for reading. The user is expected to give `r` or `rb` as the *mode* for ASCII and binary respectively.

`slow5_open_with(pathname, mode, SLOW5_FORMAT_UNKNOWN)` is equivalent to `slow5_open(pathname, mode)`.

Also see [`slow5_open()`](../slow5_open.md)

## EXAMPLES

```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "examples/example.slow5"

#define TO_PICOAMPS(RAW_VAL,DIGITISATION,OFFSET,RANGE) (((RAW_VAL)+(OFFSET))*((RANGE)/(DIGITISATION)))

int main(){

    slow5_file_t *sp = slow5_open_with(FILE_PATH,"r",SLOW5_FORMAT_ASCII);
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }
    slow5_rec_t *rec = NULL;
    int ret=0;

    ret = slow5_idx_load(sp);
    if(ret<0){
        fprintf(stderr,"Error in loading index\n");
        exit(EXIT_FAILURE);
    }

    ret = slow5_get("r3", &rec, sp);
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

    slow5_rec_free(rec);

    slow5_idx_unload(sp);

    slow5_close(sp);

}
```
