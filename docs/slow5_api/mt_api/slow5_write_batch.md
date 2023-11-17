# slow5_write_batch

## NAME

slow5_write_batch - writes a batch of SLOW5 records to a SLOW5 file

## SYNOPSYS

`int slow5_write_batch(slow5_mt_t *mt, slow5_batch_t *read_batch, int num_reads)`

## DESCRIPTION

`slow5_write_batch()` writes a batch of SLOW5 records pointed by *read_batch* to a SLOW5 file associated with *mt*.


## RETURN VALUE

The number of records written.

## ERRORS

Currently, in case of error, this function will exit the programme. Proper error handling might be introduced in future, where a negative number return is indicative of error.

## EXAMPLES

```
#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>
#include <slow5/slow5_mt.h>

#define FILE_PATH "test.blow5"

int main(){

    slow5_file_t *sf = slow5_open(FILE_PATH,"w");
    if(sf==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }

    slow5_hdr_t *header=sf->header;

    //add a header group attribute called run_id
    if (slow5_hdr_add_attr("run_id", header) != 0){
        fprintf(stderr,"Error adding run_id attribute\n");
        exit(EXIT_FAILURE);
    }

    //set the run_id attribute to "run_0" for read group 0
    if (slow5_hdr_set("run_id", "run_0", 0, header) != 0){
        fprintf(stderr,"Error setting run_id attribute in read group 0\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_hdr_write(sf) < 0){
        fprintf(stderr,"Error writing header!\n");
        exit(EXIT_FAILURE);
    }

    int ret=0;
    int batch_size = 16;
    int num_thread = 8;

    slow5_mt_t *mt = slow5_init_mt(num_thread,sf);
    if (mt==NULL){ //currently not useful, but better have this for future proofing
        fprintf(stderr,"Error in initialising multi-thread struct\n");
        exit(EXIT_FAILURE);
    }

    slow5_batch_t *read_batch = slow5_init_batch(batch_size);
    if (read_batch==NULL){ //currently not useful, but better have this for future proofing
        fprintf(stderr,"Error in initialising slow5 record batch\n");
        exit(EXIT_FAILURE);
    }

    slow5_rec_t **rec = read_batch->slow5_rec;

    for(int i=0;i<batch_size;i++){

        slow5_rec_t *slow5_record = rec[i] = slow5_rec_init();

        if(slow5_record == NULL){
            fprintf(stderr,"Could not allocate space for a slow5 record.");
            exit(EXIT_FAILURE);
        }

        char tmp_read_id[100];
        sprintf(tmp_read_id,"read_id_%d",i);
        slow5_record -> read_id = strdup(tmp_read_id);
        if(slow5_record->read_id == NULL){
            fprintf(stderr,"Could not allocate space for strdup.");
            exit(EXIT_FAILURE);
        }
        slow5_record -> read_id_len = strlen(slow5_record -> read_id);
        slow5_record -> read_group = 0;
        slow5_record -> digitisation = 4096.0;
        slow5_record -> offset = 3.0+i;
        slow5_record -> range = 10.0+i;
        slow5_record -> sampling_rate = 4000.0;
        slow5_record -> len_raw_signal = 10+i;
        slow5_record -> raw_signal = malloc(sizeof(int16_t)*(10+i));
        if(slow5_record->raw_signal == NULL){
            fprintf(stderr,"Could not allocate space for raw signal.");
            exit(EXIT_FAILURE);
        }
        for(int j=0;j<10+i;j++){
            slow5_record->raw_signal[j] = j+i;
        }

    }

    ret = slow5_write_batch(mt, read_batch,batch_size);

    if(ret!=batch_size){
        fprintf(stderr,"Writing failed\n");
        exit(EXIT_FAILURE);
    }

    slow5_free_batch(read_batch);
    slow5_free_mt(mt);
    slow5_close(sf);

    return 0;
}
```


