# slow5_write

## NAME
slow5_write - writes a SLOW5 record to a SLOW5 file.

## SYNOPSYS
`int slow5_write(slow5_rec_t *read, slow5_file_t *s5p)`

## DESCRIPTION
`slow5_write()` writes a SLOW5 record pointed by *read* to a SLOW5 file pointed by *s5p*.

The argument *s5p* points to a *slow5_file_t* opened using `slow5_open()` for writing or appending.

## RETURN VALUE
Upon successful completion, `slow5_write()` returns a non negative integer (>=0). Otherwise a negative value is returned.

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

    if(slow5_hdr_write(sp) < 0){
        fprintf(stderr,"Error writing header!\n");
        exit(EXIT_FAILURE);
    }

    slow5_rec_t *slow5_record = slow5_rec_init();
    if(slow5_record == NULL){
        fprintf(stderr,"Could not allocate space for a slow5 record.");
        exit(EXIT_FAILURE);
    }

    char *read_id = "read_1";
    int16_t raw_signal[10] = {0,1,2,3,4,5,6,7,8,9}; //a raw signal of length 10

    //set the field
    slow5_record -> read_id = read_id;
    slow5_record -> read_id_len = strlen(read_id);
    slow5_record -> read_group = 0;
    slow5_record -> digitisation = 4096.0;
    slow5_record -> offset = 4.0;
    slow5_record -> range = 12.0;
    slow5_record -> sampling_rate = 4000.0;
    slow5_record -> len_raw_signal = 10;
    slow5_record -> raw_signal = raw_signal;

    //write to file
    if (slow5_write(slow5_record, sp) < 0){
        fprintf(stderr,"Error writing record!\n");
        exit(EXIT_FAILURE);
    }

    //free memory
    //slow5_rec_free will attempt to free read_id and raw_signal arrays.
    //As we allocated them on the stack and thus they must not be freed, we must set them to NULL before calling slow5_rec_free.
    slow5_record -> read_id = NULL;
    slow5_record -> raw_signal = NULL;
    slow5_rec_free(slow5_record);

    //close the file
    slow5_close(sp);

    return 0;

}
```

