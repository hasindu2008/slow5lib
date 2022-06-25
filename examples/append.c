// an example programme that uses slow5lib to append to an existing SLOW5 file
// gcc -Wall examples/append.c  -I include/ lib/libslow5.a -lm -lz  -O2 -g

/* In this example, the following slow5 record will be appended to the file generated throgh write.c example:
#char*	    uint32_t	double	        double	double	double	        uint64_t	    int16_t*	                char*	        double	        int32_t	        uint8_t	    uint64_t
#read_id	read_group	digitisation	offset	range	sampling_rate	len_raw_signal	raw_signal	                channel_number	median_before	read_number	    start_mux	start_time
read_1	    0	        4096	        4	    12	    4000	        12	            0,1,2,3,4,5,6,7,8,9,10,11	1	            0.2	            11	            2	        200
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <slow5/slow5.h>

#define FILE_PATH "test.blow5"

int main(){

    slow5_file_t *sp = slow5_open(FILE_PATH, "a");
    if(sp==NULL){
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

    if(slow5_aux_set_string(slow5_record, "channel_number", channel_number, sp->header)!=0){
        fprintf(stderr,"Error setting channel_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    if(slow5_aux_set(slow5_record, "median_before", &median_before, sp->header)!=0){
        fprintf(stderr,"Error setting median_before auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    if(slow5_aux_set(slow5_record, "read_number", &read_number, sp->header)!=0){
        fprintf(stderr,"Error setting read_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_aux_set(slow5_record, "start_mux", &start_mux, sp->header)!=0){
        fprintf(stderr,"Error setting start_mux auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_aux_set(slow5_record, "start_time", &start_time, sp->header)!=0){
        fprintf(stderr,"Error setting start_time auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    slow5_write(slow5_record, sp);

    slow5_rec_free(slow5_record);

    slow5_close(sp);

    return 0;

}

