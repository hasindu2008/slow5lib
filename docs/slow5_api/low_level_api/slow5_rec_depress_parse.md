# slow5lib

## NAME
slow5_rec_depress_parse - Decompresses a record and then parses to a read.

## SYNOPSYS
`int slow5_rec_depress_parse(char **mem, size_t *bytes, const char *read_id, struct slow5_rec **read, struct slow5_file *s5p)`

## DESCRIPTION
This function decompresses a record fetched using `slow5_get_next_mem()`.
Decompresses record if s5p has a compression method and then parses to read.
Sets mem to decompressed mem and bytes to new bytes.

`read` should be freed using `slow5_rec_free()`

## RETURN VALUE
Upon successful completion, `slow5_rec_depress_parse()` returns 0. Otherwise, returns a `SLOW5_ERR_*` and set `slow5_errno` on failure

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

    slow5_file_t *sp = slow5_open_with(FILE_PATH,"r",SLOW5_FORMAT_ASCII);
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }
    slow5_rec_t *rec = NULL;

    size_t bytes;
    char *mem;

    if (!(mem = (char *) slow5_get_next_mem(&bytes, from))) {
        return EXIT_FAILURE;
    }

    if (slow5_rec_depress_parse(&mem, &bytes, NULL, &rec, &from) != 0) {
        exit(EXIT_FAILURE);
    } else {
        free(mem);
    }

    printf("%s\t",rec->read_id);
    uint64_t len_raw_signal = rec->len_raw_signal;
    for(uint64_t i=0;i<len_raw_signal;i++){
        double pA = TO_PICOAMPS(rec->raw_signal[i],rec->digitisation,rec->offset,rec->range);
        printf("%f ",pA);
    }
    printf("\n");

    slow5_rec_free(rec);

    slow5_close(sp);

}
```
