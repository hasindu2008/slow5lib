// an example programme that uses slow5lib to write a SLOW5 file
// gcc -Wall examples/write.c  -I include/ lib/libslow5.a -lm -lz  -O2 -g

/*
A BLOW5 file with following content will be written in this example
#num_read_groups	1
@asic_id	asic_id_0
@run_id	    run_0
#char*	    uint32_t	double      	double	double	double	        uint64_t        int16_t*	            char*	        double	        int32_t	    uint8_t	    uint64_t
#read_id	read_group	digitisation	offset	range	sampling_rate	len_raw_signal	raw_signal	            channel_number	median_before	read_number	start_mux	start_time
read_0	    0	        4096	        3	    10	    4000	        10	            0,1,2,3,4,5,6,7,8,9	    0	            0.1	            10	            1	        100
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <slow5/slow5.h>

#define FILE_PATH "test.blow5"

void set_header_attributes(slow5_file_t *sp);
void set_header_aux_fields(slow5_file_t *sp);
void set_record_primary_fields(slow5_rec_t *slow5_record, slow5_file_t *sp);
void set_record_aux_fields(slow5_rec_t *slow5_record, slow5_file_t *sp);

int main(){

    //open the SLOW5 file for writing
    slow5_file_t *sp = slow5_open(FILE_PATH, "w");
    if(sp==NULL){
        fprintf(stderr,"Error opening file!\n");
        exit(EXIT_FAILURE);
    }

    // //This section can be optionally uncommented to set non-default compression methods for blow5 (the recommended default is zlib+svb-zd)
    // if(slow5_set_press(sp, SLOW5_COMPRESS_NONE, SLOW5_COMPRESS_NONE) < 0){ // no record compression, no signal compression
    //     fprintf(stderr,"Error setting compression method!\n");
    //     exit(EXIT_FAILURE);
    // }

    /*********************** Header ******************/

    /*
    @asic_id	asic_id_0
    @run_id	    run_0
    */
    set_header_attributes(sp);

    /*
    char*	        double	        int32_t	    uint8_t	    uint64_t
    channel_number	median_before	read_number	start_mux	start_time
    */
    set_header_aux_fields(sp);

    if(slow5_hdr_write(sp) < 0){
        fprintf(stderr,"Error writing header!\n");
        exit(EXIT_FAILURE);
    }


    /******************* A SLOW5 record ************************/
    slow5_rec_t *slow5_record = slow5_rec_init();
    if(slow5_record == NULL){
        fprintf(stderr,"Could not allocate space for a slow5 record.");
        exit(EXIT_FAILURE);
    }

    /* primary fields
    #read_id	read_group	digitisation	offset	range	sampling_rate	len_raw_signal	raw_signal
    read_0	    0	        4096	        3	    10	    4000	        10	            0,1,2,3,4,5,6,7,8,9
    */
    set_record_primary_fields(slow5_record, sp);

    /* auxiliary fileds
    channel_number	median_before	read_number	start_mux	start_time
    0	            0.1	            10	            1	        100
    */
    set_record_aux_fields(slow5_record, sp);

    //write to file
    if(slow5_write(slow5_record, sp) < 0){
        fprintf(stderr,"Error writing record!\n");
        exit(EXIT_FAILURE);
    }

    //free the slow5 record
    slow5_rec_free(slow5_record);

    //close the SLOW5 file
    slow5_close(sp);

    return 0;

}

void set_header_attributes(slow5_file_t *sp){

    slow5_hdr_t *header=sp->header; //pointer to the SLOW5 header

    //add a header group attribute called run_id
    if (slow5_hdr_add("run_id", header) < 0){
        fprintf(stderr,"Error adding run_id attribute\n");
        exit(EXIT_FAILURE);
    }
    //add another header group attribute called asic_id
    if (slow5_hdr_add("asic_id", header) < 0){
        fprintf(stderr,"Error adding asic_id attribute\n");
        exit(EXIT_FAILURE);
    }

    //set the run_id attribute to "run_0" for read group 0
    if (slow5_hdr_set("run_id", "run_0", 0, header) < 0){
        fprintf(stderr,"Error setting run_id attribute in read group 0\n");
        exit(EXIT_FAILURE);
    }
    //set the asic_id attribute to "asic_0" for read group 0
    if (slow5_hdr_set("asic_id", "asic_id_0", 0, header) < 0){
        fprintf(stderr,"Error setting asic_id attribute in read group 0\n");
        exit(EXIT_FAILURE);
    }

}

void set_header_aux_fields(slow5_file_t *sp){

    //add auxilliary field: channel number
    if (slow5_aux_add("channel_number", SLOW5_STRING, sp->header) < 0){
        fprintf(stderr,"Error adding channel_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    //add auxilliary field: median_before
    if (slow5_aux_add("median_before", SLOW5_DOUBLE, sp->header) < 0) {
        fprintf(stderr,"Error adding median_before auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    //add auxilliary field: read_number
    if(slow5_aux_add("read_number", SLOW5_INT32_T, sp->header) < 0){
        fprintf(stderr,"Error adding read_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    //add auxilliary field: start_mux
    if(slow5_aux_add("start_mux", SLOW5_UINT8_T, sp->header) < 0){
        fprintf(stderr,"Error adding start_mux auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    //add auxilliary field: start_time
    if(slow5_aux_add("start_time", SLOW5_UINT64_T, sp->header) < 0){
        fprintf(stderr,"Error adding start_time auxilliary field\n");
        exit(EXIT_FAILURE);
    }

}

void set_record_primary_fields(slow5_rec_t *slow5_record, slow5_file_t *sp){

    slow5_record -> read_id = strdup("read_0");
    if(slow5_record->read_id == NULL){
        fprintf(stderr,"Could not do strdup.");
        exit(EXIT_FAILURE);
    }
    slow5_record-> read_id_len = strlen(slow5_record -> read_id);
    slow5_record -> read_group = 0;
    slow5_record -> digitisation = 4096.0;
    slow5_record -> offset = 3.0;
    slow5_record -> range = 10.0;
    slow5_record -> sampling_rate = 4000.0;
    slow5_record -> len_raw_signal = 10;
    slow5_record -> raw_signal = (int16_t *)malloc(sizeof(int16_t) * slow5_record->len_raw_signal);
    if(slow5_record->raw_signal == NULL){
        fprintf(stderr,"Could not allocate space for raw signal.");
        exit(EXIT_FAILURE);
    }
    for(int i=0; i<slow5_record->len_raw_signal; i++){
        slow5_record->raw_signal[i] = i;
    }

}

void set_record_aux_fields(slow5_rec_t *slow5_record, slow5_file_t *sp){

    char *channel_number = "0";
    double median_before = 0.1;
    int32_t read_number = 10;
    uint8_t start_mux = 1;
    uint64_t start_time = 100;

    if(slow5_aux_set_string(slow5_record, "channel_number", channel_number, sp->header) < 0){
        fprintf(stderr,"Error setting channel_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_aux_set(slow5_record, "median_before", &median_before, sp->header) < 0){
        fprintf(stderr,"Error setting median_before auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_aux_set(slow5_record, "read_number", &read_number, sp->header) < 0){
        fprintf(stderr,"Error setting read_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_aux_set(slow5_record, "start_mux", &start_mux, sp->header) < 0){
        fprintf(stderr,"Error setting start_mux auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_aux_set(slow5_record, "start_time", &start_time, sp->header) < 0){
        fprintf(stderr,"Error setting start_time auxilliary field\n");
        exit(EXIT_FAILURE);
    }

}