# slow5lib

## NAME
slow5_open - Opens a SLOW5 file.

## SYNOPSYS
`slow5_file_t *slow5_open(const char *pathname, const char *mode)`

## DESCRIPTION
Attempt to guess the file's slow5 format (ASCII and binary) from extension of the argument *pathname*. 

This function at the moment should only be used to open a file for reading. The user is expected to give `r` or `rb` as the *mode* for ASCII and binary respectively.

Since the function can detect the file type internally the user can simply give `r` as the *mode*.

An open slow5 file should be closed using `slow5_close()`


## RETURN VALUE
Upon successful completion, `slow_open()` returns a *slow5_file_t* pointer. Otherwise, NULL is returned.

## NOTES
This function at the moment should only be used to open a file for reading. The user is expected to give `r` or `rb` as the *mode* for ASCII and binary respectively.

Also see `slow5_open_with()`

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
