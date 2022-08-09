# slow5_hdr_write

## NAME

slow5_hdr_write -  writes the associated SLOW5 header to a SLOW5 file

## SYNOPSYS

`int slow5_hdr_write(slow5_file_t *s5p)`

## DESCRIPTION
`slow5_hdr_write()` writes the associated SLOW5 header to the SLOW5 file pointed by *s5p*. The associated SLOW5 header is the *s5p->header* member inside the *slow5_file_t \*s5p* returned by  `slow5_open()`.


## RETURN VALUE

Upon successful completion, `slow5_hdr_write()` returns the number of bytes written. Otherwise, a negative value is returned.

## ERRORS

todo

## NOTES

In the future a error number will be set to indicate the error.

## EXAMPLES

```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

#define FILE_PATH "test.slow5"

int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH, "w");
    if(sp==NULL){
        fprintf(stderr,"Error opening file!\n");
        exit(EXIT_FAILURE);
    }
    slow5_hdr_t* header = sp->header;

    //add a header group attribute called run_id
    if (slow5_hdr_add("run_id", header) != 0){
        fprintf(stderr,"Error adding run_id attribute\n");
        exit(EXIT_FAILURE);
    }

    //set the run_id attribute to "run_0" for read group 0
    if (slow5_hdr_set("run_id", "run_0", 0, header) != 0){
        fprintf(stderr,"Error setting run_id attribute in read group 0\n");
        exit(EXIT_FAILURE);
    }

    //add auxilliary field: channel number
    if (slow5_aux_add("channel_number", SLOW5_STRING, sp->header) < 0){
        fprintf(stderr,"Error adding channel_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_hdr_write(sp) < 0){
        fprintf(stderr,"Error writing header!\n");
        exit(EXIT_FAILURE);
    }

    slow5_close(sp);

    return 0;
}
```
