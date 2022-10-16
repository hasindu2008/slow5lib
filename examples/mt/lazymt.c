
// an example programme is for a lazy programmer. see mt.c instead.
// uses the optional multi-threaded API under  the lazy mode to fetch batches of records in parallel
// this is under construction and is yet beta

#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <slow5/slow5.h>
#include <slow5/slow5_mt.h>

#define FILE_PATH "test.blow5" //for reading
#define FILE_PATH_WRITE "test.blow5"

int read_func(){

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }
    slow5_rec_t **rec = NULL;
    int ret=0;
    int batch_size = 2048;
    int num_thread = 8;

    while(1){

        ret = slow5_get_next_batch_lazy(&rec,sp,batch_size,num_thread);
        for(int i=0;i<ret;i++){
            uint64_t len_raw_signal = rec[i]->len_raw_signal;
            printf("%s\t%ld\n",rec[i]->read_id,len_raw_signal);
        }
        slow5_free_batch_lazy(&rec,ret);

        if(ret<batch_size){ //this indicates nothing left to read //need to handle errors
            break;
        }
    }

    slow5_close(sp);


    //now random read fun
    sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }
    rec = NULL;

    ret = slow5_idx_create(sp);
    if(ret<0){
        fprintf(stderr,"Error in creating index\n");
        exit(EXIT_FAILURE);
    }

    ret = slow5_idx_load(sp);
    if(ret<0){
        fprintf(stderr,"Error in loading index\n");
        exit(EXIT_FAILURE);
    }

    int num_rid = 4;
    num_thread = 2;
    char *rid[num_rid];
    rid[0]="read_id_50";
    rid[1]="read_id_3999",
    rid[2]="read_id_0";
    rid[3]="read_id_4";

    ret = slow5_get_batch_lazy(&rec, sp, rid, num_rid, num_thread);
    assert(ret==num_rid);
    for(int i=0;i<ret;i++){
        uint64_t len_raw_signal = rec[i]->len_raw_signal;
        printf("%s\t%ld\n",rec[i]->read_id,len_raw_signal);
    }
    slow5_free_batch_lazy(&rec,ret);

    slow5_idx_unload(sp);
    slow5_close(sp);

    return 0;
}


int write_func(){

    slow5_file_t *sf = slow5_open(FILE_PATH_WRITE,"w");
    if(sf==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }

    //set zstd record compression, svb-zd signal compression
    // if(slow5_set_press(sf, SLOW5_COMPRESS_ZSTD, SLOW5_COMPRESS_SVB_ZD) < 0){ //
    //     fprintf(stderr,"Error setting compression method!\n");
    //     exit(EXIT_FAILURE);
    // }

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
    if (slow5_aux_add("channel_number", SLOW5_STRING, sf->header)!=0){
        fprintf(stderr,"Error adding channel_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    //add axuilliary field: median_before
    if (slow5_aux_add("median_before", SLOW5_DOUBLE, sf->header)!=0){
        fprintf(stderr,"Error adding median_before auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    //add axuilliary field: read_number
    if(slow5_aux_add("read_number", SLOW5_INT32_T, sf->header)!=0){
        fprintf(stderr,"Error adding read_number auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    //add axuilliary field: start_mux
    if(slow5_aux_add("start_mux", SLOW5_UINT8_T, sf->header)!=0){
        fprintf(stderr,"Error adding start_mux auxilliary field\n");
        exit(EXIT_FAILURE);
    }
    //add auxilliary field: start_time
    if(slow5_aux_add("start_time", SLOW5_UINT64_T, sf->header)!=0){
        fprintf(stderr,"Error adding start_time auxilliary field\n");
        exit(EXIT_FAILURE);
    }

    if(slow5_hdr_write(sf) < 0){
        fprintf(stderr,"Error writing header!\n");
        exit(EXIT_FAILURE);
    }


    slow5_rec_t *rec[4000];
    int ret=0;
    int batch_size = 4000;
    int num_thread = 8;


    /******************* SLOW5 records ************************/
    for(int i=0;i<batch_size;i++){

        slow5_rec_t *slow5_record = rec[i] = slow5_rec_init();

        if(slow5_record == NULL){
            fprintf(stderr,"Could not allocate space for a slow5 record.");
            exit(EXIT_FAILURE);
        }

        //primary fields
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

        //auxiliary fileds
        char *channel_number = "channel_number";
        double median_before = 0.1+i;
        int32_t read_number = 10+i;
        uint8_t start_mux = (1+i)%4;
        uint64_t start_time = 100+i;

        if(slow5_aux_set_string(slow5_record, "channel_number", channel_number, sf->header)!=0){
            fprintf(stderr,"Error setting channel_number auxilliary field\n");
            exit(EXIT_FAILURE);
        }
        if(slow5_aux_set(slow5_record, "median_before", &median_before, sf->header)!=0){
            fprintf(stderr,"Error setting median_before auxilliary field\n");
            exit(EXIT_FAILURE);
        }
        if(slow5_aux_set(slow5_record, "read_number", &read_number, sf->header)!=0){
            fprintf(stderr,"Error setting read_number auxilliary field\n");
            exit(EXIT_FAILURE);
        }

        if(slow5_aux_set(slow5_record, "start_mux", &start_mux, sf->header)!=0){
            fprintf(stderr,"Error setting start_mux auxilliary field\n");
            exit(EXIT_FAILURE);
        }

        if(slow5_aux_set(slow5_record, "start_time", &start_time, sf->header)!=0){
            fprintf(stderr,"Error setting start_time auxilliary field\n");
            exit(EXIT_FAILURE);
        }
    }
    //end of record setup

    ret = slow5_write_batch_lazy(rec,sf,batch_size,num_thread);

    if(ret<batch_size){
        fprintf(stderr,"Writing failed\n");
        exit(EXIT_FAILURE);
    }

    slow5_close(sf);

    for(int i=0;i<batch_size;i++){
        slow5_rec_free(rec[i]);
    }

    return 0;
}

int main(){

    write_func();
    read_func();


    return 0;
}


