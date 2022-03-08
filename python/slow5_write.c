#include <stdio.h>
#include <slow5/slow5.h>
#include "../src/slow5_extra.h"

slow5_file_t *slow5_open_write(char *filename, char *mode){

    FILE *fp = fopen(filename, mode);
    if(fp==NULL){
        fprintf(stderr,"Error opening file!\n");
        return NULL;
    }

    slow5_file_t *sf = slow5_init_empty(fp, filename, SLOW5_FORMAT_UNKNOWN);
    if(sf==NULL){
        fprintf(stderr,"Error initializing file!\n");
        return NULL;
    }

    slow5_hdr_t *header=sf->header;
    if (slow5_hdr_add_rg(header) < 0){
        fprintf(stderr,"Error adding read group!\n");
        return NULL;
    }
    header->num_read_groups = 1;

    struct slow5_aux_meta *aux_meta = slow5_aux_meta_init_empty();
    if(aux_meta == NULL){
        fprintf(stderr,"Error initializing aux meta!\n");
        return NULL;
    }
    header->aux_meta = aux_meta;

    if(sf->format == SLOW5_FORMAT_BINARY){
        slow5_press_method_t press_out = {SLOW5_COMPRESS_ZLIB, SLOW5_COMPRESS_SVB_ZD};
        sf->compress = slow5_press_init(press_out);
        if(!sf->compress){
            fprintf(stderr,"Could not initialise the slow5 compression method. %s","");
            return NULL;
        }
    }

    return sf;
}


int slow5_close_write(slow5_file_t *sf){
    if(sf->format == SLOW5_FORMAT_BINARY){
        if(slow5_eof_fwrite(sf->fp) < 0){
            fprintf(stderr,"Error writing EOF!\n");
            return -1;
        }
    }
    int ret = slow5_close(sf);
    return ret;
}


int slow5_header_write(slow5_file_t *sf){

    slow5_press_method_t method={SLOW5_COMPRESS_NONE,SLOW5_COMPRESS_NONE};
    if (sf->format == SLOW5_FORMAT_BINARY){
        method.record_method = sf->compress->record_press->method;
        method.signal_method = sf->compress->signal_press->method;
    }
    int ret = slow5_hdr_fwrite(sf->fp, sf->header, sf->format, method);
    return ret;
}


int slow5_rec_write(slow5_file_t *sf, slow5_rec_t *rec){
    int ret = slow5_rec_fwrite(sf->fp, rec, sf->header->aux_meta, sf->format, sf->compress);
    return ret;
}


int slow5_aux_meta_add_wrapper(slow5_hdr_t *header, const char *attr, enum slow5_aux_type type){
    int ret = slow5_aux_meta_add(header->aux_meta, attr, type);
    return ret;
}

int slow5_rec_set_wrapper(struct slow5_rec *read, slow5_hdr_t *header, const char *attr, const void *data){
    int ret = slow5_rec_set(read, header->aux_meta, attr, data);
    return ret;
}

int slow5_rec_set_string_wrapper(struct slow5_rec *read, slow5_hdr_t *header, const char *attr, const char *data) {
    int ret = slow5_rec_set_string(read, header->aux_meta, attr, data);
    return ret;
}


#ifdef DEBUG

void single_read_group_file(){

    slow5_file_t *sf = slow5_open_write("test.blow5", "w");
    if(sf==NULL){
        fprintf(stderr,"Error opening file!\n");
        exit(EXIT_FAILURE);
    }


    /*********************** Header ******************/

    slow5_hdr_t *header=sf->header;
    //add a header group attribute called run_id
    if (slow5_hdr_add_attr("run_id", header) != 0){
        fprintf(stderr,"Error adding run_id attribute\n");
        exit(EXIT_FAILURE);
    }
    //add another header group attribute called asic_id
    if (slow5_hdr_add_attr("asic_id", header) != 0){
        fprintf(stderr,"Error adding asic_id attribute\n");
        exit(EXIT_FAILURE);
    }

    //set the run_id attribute to "run_0" for read group 0
    if (slow5_hdr_set("run_id", "run_0", 0, header) != 0){
        fprintf(stderr,"Error setting run_id attribute in read group 0\n");
        exit(EXIT_FAILURE);
    }
    //set the asic_id attribute to "asic_0" for read group 1
    if (slow5_hdr_set("asic_id", "asic_id_0", 0, header) != 0){
        fprintf(stderr,"Error setting asic_id attribute in read group 0\n");
        exit(EXIT_FAILURE);
    }

    //add auxilliary field: channel number
    if (slow5_aux_meta_add(sf->header->aux_meta, "channel_number", SLOW5_STRING)!=0){
        fprintf(stderr,"Error adding channel_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    //add axuilliary field: median_before
    if (slow5_aux_meta_add(sf->header->aux_meta, "median_before", SLOW5_DOUBLE)!=0){
        fprintf(stderr,"Error adding median_before auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    //add axuilliary field: read_number
    if(slow5_aux_meta_add_wrapper(sf->header, "read_number", SLOW5_INT32_T)!=0){
        fprintf(stderr,"Error adding read_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    //add axuilliary field: start_mux
    if(slow5_aux_meta_add_wrapper(sf->header, "start_mux", SLOW5_UINT8_T)!=0){
        fprintf(stderr,"Error adding start_mux auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    //add auxilliary field: start_time
    if(slow5_aux_meta_add_wrapper(sf->header, "start_time", SLOW5_UINT64_T)!=0){
        fprintf(stderr,"Error adding start_time auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_header_write(sf) < 0){
        fprintf(stderr,"Error writing header!\n");
        exit(EXIT_FAILURE);
    }


    /******************* A SLOW5 record ************************/
    slow5_rec_t *slow5_record = slow5_rec_init();
    if(slow5_record == NULL){
        fprintf(stderr,"Could not allocate space for a slow5 record.");
        exit(EXIT_FAILURE);
    }

    //primary fields
    slow5_record -> read_id = strdup("read_0");
    if(slow5_record->read_id == NULL){
        fprintf(stderr,"Could not allocate space for strdup.");
        exit(EXIT_FAILURE);
    }
    slow5_record -> read_id_len = strlen(slow5_record -> read_id);
    slow5_record -> read_group = 0;
    slow5_record -> digitisation = 4096.0;
    slow5_record -> offset = 3.0;
    slow5_record -> range = 10.0;
    slow5_record -> sampling_rate = 4000.0;
    slow5_record -> len_raw_signal = 10;
    slow5_record -> raw_signal = malloc(sizeof(int16_t)*10);
    if(slow5_record->raw_signal == NULL){
        fprintf(stderr,"Could not allocate space for raw signal.");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<10;i++){
        slow5_record->raw_signal[i] = i;
    }

    //auxiliary fileds
    char *channel_number = "channel_number";
    double median_before = 0.1;
    int32_t read_number = 10;
    uint8_t start_mux = 1;
    uint64_t start_time = 100;

    if(slow5_rec_set_string_wrapper(slow5_record, sf->header, "channel_number", channel_number)!=0){
        fprintf(stderr,"Error setting channel_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    if(slow5_rec_set_wrapper(slow5_record, sf->header, "median_before", &median_before)!=0){
        fprintf(stderr,"Error setting median_before auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    if(slow5_rec_set_wrapper(slow5_record, sf->header, "read_number", &read_number)!=0){
        fprintf(stderr,"Error setting read_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_rec_set_wrapper(slow5_record, sf->header, "start_mux", &start_mux)!=0){
        fprintf(stderr,"Error setting start_mux auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_rec_set_wrapper(slow5_record, sf->header, "start_time", &start_time)!=0){
        fprintf(stderr,"Error setting start_time auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    slow5_rec_write(sf, slow5_record);

    slow5_rec_free(slow5_record);

    slow5_close_write(sf);

}


int main(){


    single_read_group_file();


    return EXIT_SUCCESS;


}

#endif
//gcc -Wall python/slow5_write.c  -I include/ lib/libslow5.a -lm -lz  -g -D DEBUG=1
