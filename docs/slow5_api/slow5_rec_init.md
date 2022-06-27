# slow5_rec_init

## NAME
slow5_rec_init - initialises an empty SLOW5 record

## SYNOPSYS
`slow5_rec_t *slow5_rec_init(void)`

## DESCRIPTION

The `slow5_rec_init()` function initialises and returns an empty SLOW5 record. The returned SLOW5 record must be later freed by calling `slow5_rec_free()`.

## RETURN VALUE

Upon successful completion, `slow5_rec_init()` returns a *slow5_rec_t* pointer. Otherwise NULL is returned.

## NOTES

In a slow5 record returned by `slow5_rec_init()`, the user can directly set the primary fields as shown in the examples below.


## EXAMPLES

```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>

int main(){

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

    // do whatever you want with the record
    // ...

    //slow5_rec_free will attempt to free read_id and raw_signal arrays.
    //As we allocated them on the stack and thus they must not be freed, we must set them to NULL before calling slow5_rec_free.
    slow5_record -> read_id = NULL;
    slow5_record -> raw_signal = NULL;
    slow5_rec_free(slow5_record);

}

```

## SEE ALSO

[slow5_rec_free()](slow5_rec_free.md)
