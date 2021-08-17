# slow5_get_next

## NAME
slow5_get_next - fetches the record at the current file pointer of a slow5 file

## SYNOPSYS
`int slow5_get_next(slow5_rec_t **read, slow5_file_t *s5p)`

## DESCRIPTION
`slow5_get_next()` fetches a record from a SLOW5 file *s5p* at the current file pointer into a *slow5_rec_t* and stores the address of the *slow5_rec_t* in **read*.

If **read* is set to NULL before the call, then `slow5_get_next()` will allocate a *slow5_rec_t* for storing the record.
This *slow5_rec_t* should be freed by the user program using `slow5_rec_free()`.
Alternatively, before calling `slow5_get_next()`, **read* can contain a pointer to an allocated *slow5_rec_t* from a previous `slow5_get_next()` call.
If the allocated *slow5_rec_t* is not large enough to hold the record, `slow5_get_next()` will resize it internally.

The argument *s5p* points to a *slow5_file_t* opened using `slow5_open()`.

## RETURN VALUE
Upon successful completion, `slow5_get_next()` returns a non negative integer (>=0). Otherwise (including the end of file), a negative value is returned that indicates the error and `slow5_errno` is set to indicate the error.

## ERRORS

* `SLOW5_ERR_EOF`      
   &nbsp;&nbsp;&nbsp;&nbsp; End of file (EOF) (if blow5, EOF marker was properly found at the end).
* `SLOW5_ERR_ARG`      
   &nbsp;&nbsp;&nbsp;&nbsp; Invalid argument - Read or s5p is NULL.
* `SLOW5_ERR_RECPARSE`  
   &nbsp;&nbsp;&nbsp;&nbsp; Record parsing error.
* `SLOW5_ERR_IO`       
   &nbsp;&nbsp;&nbsp;&nbsp; I/O error (except EOF) when reading the slow5 file, for instance, `getline()` or `fread()` failed.
* `SLOW5_ERR_MEM`        
   &nbsp;&nbsp;&nbsp;&nbsp; Memory allocation error.
* `SLOW5_ERR_PRESS`      
  &nbsp;&nbsp;&nbsp;&nbsp; Record decompression error.
* `SLOW5_ERR_TRUNC`      
  &nbsp;&nbsp;&nbsp;&nbsp; Truncated blow5 file - EOF reached without seeing the blow5 EOF marker. 
*  `SLOW5_ERR_UNK`        
  &nbsp;&nbsp;&nbsp;&nbsp; Slow5 format is unknown.

## NOTES
 `slow5_get_next` does not require an index to be pre-loaded, as opposed to `slow5_get()` which so.

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

    while((ret = slow5_get_next(&rec,sp)) >= 0){
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

[slow5_get()](slow5_open.md)
