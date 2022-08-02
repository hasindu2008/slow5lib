# slow5lib

## NAME
slow_decode - Decompresses a record and then parses to a read.

## SYNOPSYS
`int slow_decode(void **mem, size_t *bytes, slow5_rec_t **read, slow5_file_t *s5p);`

## DESCRIPTION
This function decompresses a record fetched using `slow5_get_next_bytes()`.
Decompresses record if s5p has a compression method and then parses to read.
Sets mem to decompressed mem and bytes to new bytes.

`read` should be freed using `slow5_rec_free()`

## RETURN VALUE
Upon successful completion, `slow_decode()` returns 0. Otherwise, returns a `SLOW5_ERR_*` and set `slow5_errno` on failure

## NOTES

Does not free `mem` buffer.

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
