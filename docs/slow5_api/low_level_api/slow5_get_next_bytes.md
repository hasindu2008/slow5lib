# slow5_get_next_bytes

## NAME
slow5_get_next_bytes - fetches the raw record (without decompressing or parsing) at the current file pointer of a slow5 file

## SYNOPSYS
`int slow5_get_next_bytes(char **mem, size_t *bytes, slow5_file_t *s5p);`

## DESCRIPTION

`slow5_get_next_bytes()` fetches a raw record (as it is on the disk without decompressing or parsing) from a SLOW5 file *s5p* at the current file pointer into a *char** buffer and stores the address of this buffer in **mem*.

he size of the fetched record (in bytes) will be stored at **bytes*. The **mem* buffer should be always freed by the user program using `free()`. The argument *s5p* points to a *slow5_file_t* opened using `slow5_open()`.


## RETURN VALUE
Upon successful completion, `slow5_get_next_bytes()` returns a non negative integer (>=0). Otherwise (including the end of file), a negative value is returned that indicates the error and `slow5_errno` is set to indicate the error.

## ERRORS

* `SLOW5_ERR_EOF`
   &nbsp;&nbsp;&nbsp;&nbsp; End of file (EOF) (if blow5, EOF marker was properly found at the end).
* `SLOW5_ERR_IO`
   &nbsp;&nbsp;&nbsp;&nbsp; I/O error (except EOF) when reading the slow5 file, for instance, `getline()` or `fread()` failed.
* `SLOW5_ERR_MEM`
   &nbsp;&nbsp;&nbsp;&nbsp; Memory allocation error.
* `SLOW5_ERR_TRUNC`
  &nbsp;&nbsp;&nbsp;&nbsp; Truncated blow5 file - EOF reached without seeing the blow5 EOF marker.
*  `SLOW5_ERR_UNK`
  &nbsp;&nbsp;&nbsp;&nbsp; Slow5 format is unknown.

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

[slow5_decode()](slow5_decode.md)
