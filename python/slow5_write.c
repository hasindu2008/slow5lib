#include <stdio.h>
#include <slow5/slow5.h>
#include "../src/slow5_extra.h"

extern enum slow5_log_level_opt  slow5_log_level;
extern enum slow5_exit_condition_opt  slow5_exit_condition;

slow5_file_t *slow5_open_write(const char *filename, const char *mode){

    FILE *fp = fopen(filename, "w");
    if(fp==NULL){
        SLOW5_ERROR_EXIT("Error opening file '%s': %s.", filename, strerror(errno));
        slow5_errno = SLOW5_ERR_IO;
        return NULL;
    }

    slow5_file_t *sf = slow5_init_empty(fp, filename, SLOW5_FORMAT_UNKNOWN);
    if(sf==NULL){
        SLOW5_ERROR_EXIT("Error initialising an empty SLOW5 file '%s'",filename); //todo error handling down the chain
        fclose(fp);
        return NULL;
    }

    slow5_hdr_t *header=sf->header;
    if (slow5_hdr_add_rg(header) < 0){
        SLOW5_ERROR_EXIT("Error adding read group 0 for %s",filename); //todo error handling down the chain
        slow5_close(sf);
        return NULL;
    }
    header->num_read_groups = 1;

    struct slow5_aux_meta *aux_meta = slow5_aux_meta_init_empty();
    if(aux_meta == NULL){
        SLOW5_ERROR_EXIT("Error initializing aux meta for %s",filename); //todo error handling down the chain
        slow5_close(sf);
        return NULL;
    }
    header->aux_meta = aux_meta;

    //not thread safe, this structure is only used in single threaded writes
    if(sf->format == SLOW5_FORMAT_BINARY){
        slow5_press_method_t press_out = {SLOW5_COMPRESS_ZLIB, SLOW5_COMPRESS_SVB_ZD};
        sf->compress = slow5_press_init(press_out);
        if(!sf->compress){
            SLOW5_ERROR_EXIT("Could not initialise the slow5 compression method. %s","");
            slow5_close(sf);
            return NULL;
        }
    }

    return sf;
}

slow5_file_t *slow5_open_write_append(const char *filename, const char *mode){

    slow5_file_t* slow5File = slow5_open(filename, "r");
    if(!slow5File){
        SLOW5_ERROR_EXIT("Error opening file '%s': %s.", filename, strerror(errno));
        slow5_errno = SLOW5_ERR_IO;
        return NULL;
    }

    if(slow5File->format==SLOW5_FORMAT_BINARY){
        if(fseek(slow5File->fp , 0, SEEK_END) !=0 ){
            SLOW5_ERROR_EXIT("Fseek to the end of file failed '%s': %s.", filename, strerror(errno));
            slow5_errno = SLOW5_ERR_IO;
            slow5_close(slow5File);
            return NULL;
        }
        const char eof[] = SLOW5_BINARY_EOF;
        if(slow5_is_eof(slow5File->fp, eof, sizeof eof)!=1){
            SLOW5_ERROR_EXIT("No valid slow5 EOF marker at the end of the SLOW5 file %s.",filename); //should be a warning instead?
            //todo error code
            slow5_close(slow5File);
            return NULL;
        }
    }

    int ret = fclose(slow5File->fp);
    if(ret != 0){
        SLOW5_ERROR_EXIT("Closing %s after reading the header failed\n", filename);
        //todo free what is inside slow5File
        slow5_errno = SLOW5_ERR_IO;
        return NULL;
    }

    //opening a file like this twice is unnecessary if we use open_with, but lazy for now
    slow5File->fp = fopen(filename, "r+");
    if(slow5File->fp == NULL){
        SLOW5_ERROR_EXIT("Error opening file for appending'%s': %s.", filename, strerror(errno));
        slow5_errno = SLOW5_ERR_IO;
         //todo free what is inside slow5File
        return NULL;
    }

    if(slow5File->format==SLOW5_FORMAT_BINARY){
        const char eof[] = SLOW5_BINARY_EOF;
        if(fseek(slow5File->fp, - (sizeof *eof) * (sizeof eof) , SEEK_END) != 0){
            SLOW5_ERROR_EXIT("Fseek to the end of file failed '%s': %s.", filename, strerror(errno));
            slow5_errno = SLOW5_ERR_IO;
            slow5_close(slow5File);
            return NULL;
        }
    } else if (slow5File->format==SLOW5_FORMAT_ASCII){
        if(fseek(slow5File->fp, 0 , SEEK_END) != 0){
            SLOW5_ERROR_EXIT("Fseek to the end of file failed '%s': %s.", filename, strerror(errno));
            slow5_errno = SLOW5_ERR_IO;
            slow5_close(slow5File);
            return NULL;
        }
    } else {
        SLOW5_ERROR("Unknown slow5 format for file '%s'. Extension must be '%s' or '%s'.",
                filename, SLOW5_ASCII_EXTENSION, SLOW5_BINARY_EXTENSION);
        slow5_errno = SLOW5_ERR_UNK;
        slow5_close(slow5File);
        return NULL;
    }

    return slow5File;

}


int slow5_close_write(slow5_file_t *sf){
    if(sf->format == SLOW5_FORMAT_BINARY){
        if(slow5_eof_fwrite(sf->fp) < 0){
            SLOW5_ERROR_EXIT("%s","Error writing EOF!\n");
            //todo free sf
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


#ifdef PYSLOW5_WRITE_DEBUG

#define FILE_NAME "test.slow5"

void single_read_group_file(){

    slow5_file_t *sf = slow5_open_write(FILE_NAME, "w");
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


    // // now some appending fun

    sf = slow5_open_write_append(FILE_NAME, "a");
    if(sf==NULL){
        fprintf(stderr,"Error opening file!\n");
        exit(EXIT_FAILURE);
    }

    /******************* A SLOW5 record ************************/
    slow5_record = slow5_rec_init();
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
    slow5_record -> raw_signal = malloc(sizeof(int16_t)*12);
    if(slow5_record->raw_signal == NULL){
        fprintf(stderr,"Could not allocate space for raw signal.");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<12;i++){
        slow5_record->raw_signal[i] = i;
    }

    //auxiliary fileds
    channel_number = "channel_number";
    median_before = 0.2;
    read_number = 11;
    start_mux = 2;
    start_time = 200;

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
//gcc -Wall python/slow5_write.c  -I include/ lib/libslow5.a -lm -lz  -O2 -g -D PYSLOW5_WRITE_DEBUG=1
