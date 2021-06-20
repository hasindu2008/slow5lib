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

Also see [`slow5_open()`](slow5_open.md) and [`slow5_open_with()`](low_level_api/slow5_open_with.md).

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
