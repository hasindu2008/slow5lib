# slow5_xx

## NAME

slow5_xx - fetches a record from a SLOW5 file corresponding to a given read ID

## SYNOPSYS

`int slow5_xx(const char *read_id, slow5_rec_t **read, slow5_file_t *s5p)`

## DESCRIPTION

`slow5_xx()` fetches a record from a SLOW5 file *s5p* for a specified *read_id* into a *slow5_rec_t* and stores the address of the *slow5_rec_t* in **read*.


## RETURN VALUE

Upon successful completion, `slow5_xx()` returns a non negative integer (>=0). Otherwise, a negative value is returned that indicates the error and `slow5_errno` is set to indicate the error.

## ERRORS

* `SLOW5_ERR_ARG`       
    &nbsp;&nbsp;&nbsp;&nbsp; Invalid argument - read_id, read or s5p is NULL.


## NOTES

`slow5_xx()` internally uses `pread()`.

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
        fprintf(stderr,"Error when fetching the read\n");
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

## SEE ALSO
[slow5_yy()](slow5_yy.md), [slow5_yy()](slow5_yy.md)
