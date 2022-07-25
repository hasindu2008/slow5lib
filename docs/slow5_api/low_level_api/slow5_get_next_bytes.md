# slow5lib

## NAME
slow5_get_next_bytes - Gets next slow5 record from current file pointer as it is stored in s5p with length stored at pointer n.

## SYNOPSYS
`int slow5_get_next_bytes(void **mem, size_t *bytes, slow5_file_t *s5p);`

## DESCRIPTION
This function returns the next slow5 record as it is in a void buffer.

Returned buffer should be freed using `free()`

## RETURN VALUE
Upon successful completion, `slow5_get_next_bytes()` returns a *void* pointer. Otherwise, NULL is returned, e.g., if *s5p* is NULL.


## NOTES
On error following errors are set
 * slow5_errno errors:
 * SLOW5_ERR_ARG
 * SLOW5_ERR_IO
 * SLOW5_ERR_EOF    end of file reached (and blow5 eof marker found)
 * slow5_is_eof errors:
 * SLOW5_ERR_TRUNC
 * SLOW5_ERR_MEM
 * SLOW5_ERR_UNK

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
    void *mem = NULL;
    size_t bytes = 0;

    while((ret = slow5_get_next_bytes(&mem,&bytes,sp)) >= 0){
        if(slow_decode(&mem, &bytes, &rec, sp) < 0){
            fprintf(stderr,"Error in decoding record\n");
            exit(EXIT_FAILURE);
        }
        free(mem);
        printf("%s\t",rec->read_id);
        uint64_t len_raw_signal = rec->len_raw_signal;
        for(uint64_t i=0;i<len_raw_signal;i++){
            double pA = TO_PICOAMPS(rec->raw_signal[i],rec->digitisation,rec->offset,rec->range);
            printf("%f ",pA);
        }
        printf("\n");
    }

    if(ret != SLOW5_ERR_EOF){  //check if proper end of file has been reached
        fprintf(stderr,"Error in slow5_get_next. Error code %d\n",ret);
        exit(EXIT_FAILURE);
    }

    slow5_rec_free(rec);

    slow5_close(sp);

}
```
