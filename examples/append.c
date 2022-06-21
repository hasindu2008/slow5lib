// an example programme that uses slow5lib to append to an existing SLOW5 file
// gcc -Wall examples/append.c  -I include/ lib/libslow5.a -lm -lz  -O2 -g

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <slow5/slow5.h>

#define FILE_NAME "test.blow5"

int main(){

    slow5_file_t *sf = slow5_open(FILE_NAME, "a");
    if(sf==NULL){
        fprintf(stderr,"Error opening file!\n");
        exit(EXIT_FAILURE);
    }

    /******************* A SLOW5 record ************************/
    slow5_rec_t *slow5_record = slow5_rec_init();
    if(slow5_record == NULL){
        fprintf(stderr,"Could not allocate space for a slow5 record.");
        exit(EXIT_FAILURE);
    }

    //primary fields
    slow5_record -> read_id = strdup("read_1");
    if(slow5_record->read_id == NULL){
        fprintf(stderr,"Could not allocate space for strdup.");
        exit(EXIT_FAILURE);
    }
    slow5_record -> read_id_len = strlen(slow5_record -> read_id);
    slow5_record -> read_group = 0;
    slow5_record -> digitisation = 4096.0;
    slow5_record -> offset = 4.0;
    slow5_record -> range = 12.0;
    slow5_record -> sampling_rate = 4000.0;
    slow5_record -> len_raw_signal = 12;
    slow5_record -> raw_signal = malloc(sizeof(int16_t) * slow5_record -> len_raw_signal);
    if(slow5_record->raw_signal == NULL){
        fprintf(stderr,"Could not allocate space for raw signal.");
        exit(EXIT_FAILURE);
    }
    for(int i=0; i<slow5_record->len_raw_signal; i++){
        slow5_record->raw_signal[i] = i;
    }

    //auxiliary fileds
    char *channel_number = "1";
    double median_before = 0.2;
    int32_t read_number = 11;
    uint8_t start_mux = 2;
    uint64_t start_time = 200;

    if(slow5_aux_set_string(slow5_record, sf->header, "channel_number", channel_number)!=0){
        fprintf(stderr,"Error setting channel_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    if(slow5_aux_set(slow5_record, sf->header, "median_before", &median_before)!=0){
        fprintf(stderr,"Error setting median_before auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    if(slow5_aux_set(slow5_record, sf->header, "read_number", &read_number)!=0){
        fprintf(stderr,"Error setting read_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_aux_set(slow5_record, sf->header, "start_mux", &start_mux)!=0){
        fprintf(stderr,"Error setting start_mux auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_aux_set(slow5_record, sf->header, "start_time", &start_time)!=0){
        fprintf(stderr,"Error setting start_time auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    slow5_write(sf, slow5_record);

    slow5_rec_free(slow5_record);

    slow5_close(sf);

    return 0;

}

