# slow5_decode

## NAME
slow5_decode -  decodes a slow5 record

## SYNOPSYS
`int slow5_decode(char **mem, size_t *bytes, slow5_rec_t **read, slow5_file_t *s5p);`

## DESCRIPTION
This function decode a raw record pointed by **mem*  of size *bytes* (previously fetched using `slow5_get_next_bytes()`) into a *slow5_rec_t* and stores the address of the *slow5_rec_t* in **read*. The argument *s5p* points to a *slow5_file_t* opened using `slow5_open()`.

If **read* is set to NULL before the call, then `slow5_get_next()` will allocate a *slow5_rec_t* for storing the record.
This *slow5_rec_t* should be freed by the user program using `slow5_rec_free()`.
Alternatively, before calling `slow5_get_next()`, **read* can contain a pointer to an allocated *slow5_rec_t* from a previous `slow5_get_next()` call.
If the allocated *slow5_rec_t* is not large enough to hold the record, `slow5_get_next()` will resize it internally.

## RETURN VALUE

Upon successful completion, `slow5_decode()` returns a non negative integer (>=0). Otherwise, a negative value is returned that indicates the error and `slow5_errno` is set to indicate the error.

## ERRORS

* `SLOW5_ERR_RECPARSE`
   &nbsp;&nbsp;&nbsp;&nbsp; Record parsing error.
* `SLOW5_ERR_MEM`
   &nbsp;&nbsp;&nbsp;&nbsp; Memory allocation error.
* `SLOW5_ERR_PRESS`
  &nbsp;&nbsp;&nbsp;&nbsp; Record decompression error.

## NOTES

If the file is compressed binary, the raw record will be first decompressed and before parsing.

This function does NOT free the `*mem` buffer.

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
    char *mem = NULL;
    size_t bytes = 0;

    while((ret = slow5_get_next_bytes(&mem,&bytes,sp)) >= 0){
        if(slow5_decode(&mem, &bytes, &rec, sp) < 0){
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

## SEE ALSO
[slow5_get_next_bytes()](slow5_get_next_bytes.md).