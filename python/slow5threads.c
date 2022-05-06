/* @file slow5threads.c
**
** @@
******************************************************************************/

#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <slow5/slow5.h>
#include "../src/slow5_extra.h"
#include "slow5_write.h"

#define SLOW5_WORK_STEAL 1 //simple work stealing enabled or not (no work stealing mean no load balancing)
#define SLOW5_STEAL_THRESH 1 //stealing threshold

extern enum slow5_log_level_opt  slow5_log_level;
extern enum slow5_exit_condition_opt  slow5_exit_condition;

#define SLOW5_MALLOC_CHK_LAZY_EXIT(ret) { \
    SLOW5_MALLOC_CHK(ret) \
    if (ret == NULL) { \
        exit(EXIT_FAILURE); \
    } \
}

/* a batch of read data (dynamic data based on the reads) */
typedef struct {
    int32_t n_rec;
    int32_t capacity_rec;

    char **mem_records; //unused in get()
    size_t *mem_bytes;

    slow5_rec_t **slow5_rec;
    char **rid; //only used in get()

} slow5_db_t;


/* core data structure (mostly static data throughout the program lifetime) */
typedef struct {
    //slow5
    slow5_file_t *sf;
    int num_thread;
    int batch_size;
} slow5_core_t;


/* argument wrapper for the multithreaded framework used for data processing */
typedef struct {
    slow5_core_t* core;
    slow5_db_t* db;
    int32_t starti;
    int32_t endi;
    void (*func)(slow5_core_t*,slow5_db_t*,int);
    int32_t thread_index;
#ifdef SLOW5_WORK_STEAL
    void *all_pthread_args;
#endif
} slow5_pt_arg_t;


/* initialise the core data structure */
static slow5_core_t* slow5_init_core(slow5_file_t *s5p, int batch_size, int num_thread) {

    slow5_core_t* core = (slow5_core_t*)malloc(sizeof(slow5_core_t));
    SLOW5_MALLOC_CHK_LAZY_EXIT(core);

    core->sf = s5p;
    core->batch_size = batch_size;
    core->num_thread = num_thread;

    return core;
}

/* free the core data structure */
static void slow5_free_core(slow5_core_t* core) {
    free(core);
}

/* initialise a data batch */
static slow5_db_t* slow5_init_db(slow5_core_t* core) {
    slow5_db_t* db = (slow5_db_t*)(malloc(sizeof(slow5_db_t)));
    SLOW5_MALLOC_CHK_LAZY_EXIT(db);

    db->capacity_rec = core->batch_size;
    db->n_rec = 0;

    db->mem_records = (char**)(calloc(db->capacity_rec,sizeof(char*)));
    SLOW5_MALLOC_CHK_LAZY_EXIT(db->mem_records);
    db->mem_bytes = (size_t*)(calloc(db->capacity_rec,sizeof(size_t)));
    SLOW5_MALLOC_CHK_LAZY_EXIT(db->mem_bytes);

    db->slow5_rec = (slow5_rec_t**)calloc(db->capacity_rec,sizeof(slow5_rec_t*));
    SLOW5_MALLOC_CHK_LAZY_EXIT(db->slow5_rec);

    return db;
}

/* load a data batch from disk */
static int slow5_load_db(slow5_core_t* core, slow5_db_t* db) {

    db->n_rec = 0;

    int32_t i = 0;
    while (db->n_rec < db->capacity_rec) {
        i=db->n_rec;
        db->mem_records[i] = (char *)slow5_get_next_mem(&(db->mem_bytes[i]), core->sf);

        if (db->mem_records[i] == NULL) {
            if (slow5_errno != SLOW5_ERR_EOF) {
                SLOW5_ERROR("Error reading from SLOW5 file %d\n", slow5_errno);
                exit(EXIT_FAILURE);
            }
            else {
                SLOW5_LOG_DEBUG("%s","Last Batch!\n");
                break;
            }
        }
        else {
            db->n_rec++;
        }
    }

    return db->n_rec;
}


static int slow5_write_db(slow5_core_t* core, slow5_db_t* db) {


    int32_t i = 0;
    for(i=0;i<db->n_rec;i++) {

        size_t n = fwrite(db->mem_records[i], db->mem_bytes[i], 1, core->sf->fp);
        if (n != 1) {
            SLOW5_ERROR("Writing failed for read id %s!\n", db->slow5_rec[i]->read_id);
        }

    }

    return i;
}


static void slow5_parse_single(slow5_core_t* core,slow5_db_t* db, int32_t i){

    assert(db->mem_bytes[i]>0);
    assert(db->mem_records[i]!=NULL);
    int ret=slow5_rec_depress_parse(&db->mem_records[i], &db->mem_bytes[i], NULL, &db->slow5_rec[i], core->sf);
    if(ret!=0){
        SLOW5_ERROR("Error parsing the record %s",db->slow5_rec[i]->read_id);
        exit(EXIT_FAILURE);
    }

}


static void slow5_work_per_single_read(slow5_core_t* core,slow5_db_t* db, int32_t i){
    slow5_parse_single(core,db,i);
}

static void slow5_work_per_single_read2(slow5_core_t* core,slow5_db_t* db, int32_t i){
    assert(db->rid[i]!=NULL);
    int ret = slow5_get(db->rid[i],&db->slow5_rec[i], core->sf);
    if(ret<0){
        SLOW5_ERROR("Error when fetching the read %s\n",db->rid[i]);
        exit(EXIT_FAILURE);
    }
    db->mem_bytes[i]=ret;

}

static void slow5_work_per_single_read3(slow5_core_t* core,slow5_db_t* db, int32_t i){
    assert(db->slow5_rec[i]!=NULL);
    slow5_file_t *sf = core->sf;
    //fprintf(stderr,"Here %d\n",i);
    slow5_press_method_t press_out = {SLOW5_COMPRESS_ZLIB, SLOW5_COMPRESS_SVB_ZD};
    slow5_press_t *press_ptr = slow5_press_init(press_out);
    if(!press_ptr){
        SLOW5_ERROR("Could not initialize the slow5 compression method%s","");
        exit(EXIT_FAILURE);
    }
    db->mem_records[i] = slow5_rec_to_mem(db->slow5_rec[i], sf->header->aux_meta, sf->format, press_ptr, &(db->mem_bytes[i]));
    //fprintf(stderr,"Here 2 %d\n",i);
    slow5_press_free(press_ptr);

    if(db->mem_records[i] == NULL){
        SLOW5_ERROR("Error when converting the read %d to memory\n",i);
        exit(EXIT_FAILURE);
    }

}


/* partially free a data batch - only the read dependent allocations are freed */
static void slow5_free_db_tmp(slow5_db_t* db) {
    int32_t i = 0;
    for (i = 0; i < db->n_rec; ++i) {
        free(db->mem_records[i]);
    }
}

/* completely free a data batch */
static void slow5_free_db(slow5_db_t* db) {

    free(db->mem_records);
    free(db->mem_bytes);;

    free(db);
}


static inline int32_t steal_work(slow5_pt_arg_t* all_args, int32_t num_thread) {
	int32_t i, c_i = -1;
	int32_t k;
	for (i = 0; i < num_thread; ++i){
        slow5_pt_arg_t args = all_args[i];
        //fprintf(stderr,"endi : %d, starti : %d\n",args.endi,args.starti);
		if (args.endi-args.starti > SLOW5_STEAL_THRESH) {
            //fprintf(stderr,"gap : %d\n",args.endi-args.starti);
            c_i = i;
            break;
        }
    }
    if(c_i<0){
        return -1;
    }
	k = __sync_fetch_and_add(&(all_args[c_i].starti), 1);
    //fprintf(stderr,"k : %d, end %d, start %d\n",k,all_args[c_i].endi,all_args[c_i].starti);
	return k >= all_args[c_i].endi ? -1 : k;
}


static void* slow5_pthread_single(void* voidargs) {
    int32_t i;
    slow5_pt_arg_t* args = (slow5_pt_arg_t*)voidargs;
    slow5_db_t* db = args->db;
    slow5_core_t* core = args->core;

#ifndef SLOW5_WORK_STEAL
    for (i = args->starti; i < args->endi; i++) {
        args->func(core,db,i);
    }
#else
    slow5_pt_arg_t* all_args = (slow5_pt_arg_t*)(args->all_pthread_args);
    //adapted from kthread.c in minimap2
    for (;;) {
		i = __sync_fetch_and_add(&args->starti, 1);
		if (i >= args->endi) {
            break;
        }
		args->func(core,db,i);
	}
	while ((i = steal_work(all_args,core->num_thread)) >= 0){
		args->func(core,db,i);
    }
#endif

    //fprintf(stderr,"Thread %d done\n",(myargs->position)/THREADS);
    pthread_exit(0);
}

static void slow5_pthread_db(slow5_core_t* core, slow5_db_t* db, void (*func)(slow5_core_t*,slow5_db_t*,int)){
    //create threads
    pthread_t tids[core->num_thread];
    slow5_pt_arg_t pt_args[core->num_thread];
    int32_t t, ret;
    int32_t i = 0;
    int32_t num_thread = core->num_thread;
    int32_t step = (db->n_rec + num_thread - 1) / num_thread;
    //todo : check for higher num of threads than the data
    //current works but many threads are created despite

    SLOW5_LOG_DEBUG("Creating %d threads\n",num_thread);
    //set the data structures
    for (t = 0; t < num_thread; t++) {
        pt_args[t].core = core;
        pt_args[t].db = db;
        pt_args[t].starti = i;
        i += step;
        if (i > db->n_rec) {
            pt_args[t].endi = db->n_rec;
        } else {
            pt_args[t].endi = i;
        }
        pt_args[t].func=func;
    #ifdef SLOW5_WORK_STEAL
        pt_args[t].all_pthread_args =  (void *)pt_args;
    #endif
        //fprintf(stderr,"t%d : %d-%d\n",t,pt_args[t].starti,pt_args[t].endi);

    }

    //create threads
    for(t = 0; t < core->num_thread; t++){
        ret = pthread_create(&tids[t], NULL, slow5_pthread_single,
                                (void*)(&pt_args[t]));
        if(ret < 0){
            SLOW5_ERROR("Error creating thread %d\n",t);
            exit(EXIT_FAILURE);
        }
    }

    //pthread joining
    for (t = 0; t < core->num_thread; t++) {
        int ret = pthread_join(tids[t], NULL);
        if(ret < 0){
            SLOW5_ERROR("Error creating thread %d\n",t);
            exit(EXIT_FAILURE);
        }
    }
}

/* process all reads in the given batch db */
static void slow5_work_db(slow5_core_t* core, slow5_db_t* db, void (*func)(slow5_core_t*,slow5_db_t*,int)){

    if (core->num_thread == 1) {
        int32_t i=0;
        for (i = 0; i < db->n_rec; i++) {
            func(core,db,i);
        }

    }

    else {
        slow5_pthread_db(core,db,func);
    }
}

int slow5_get_batch(slow5_rec_t ***read, slow5_file_t *s5p, char **rid, int num_rid, int num_threads){

    slow5_core_t *core = slow5_init_core(s5p,num_rid,num_threads);
    slow5_db_t* db = slow5_init_db(core);

    db->rid = rid;
    db->n_rec = num_rid;
    slow5_work_db(core,db,slow5_work_per_single_read2);
    SLOW5_LOG_DEBUG("loaded and parsed %d recs\n",num_rid);

    *read = db->slow5_rec;

    slow5_free_db_tmp(db);
    slow5_free_db(db);
    slow5_free_core(core);

    return num_rid;
}


int slow5_get_next_batch(slow5_rec_t ***read, slow5_file_t *s5p, int batch_size, int num_threads){

    slow5_core_t *core = slow5_init_core(s5p,batch_size,num_threads);
    slow5_db_t* db = slow5_init_db(core);

    int num_read=slow5_load_db(core,db);
    SLOW5_LOG_DEBUG("Loaded %d recs\n",num_read);
    slow5_work_db(core,db,slow5_work_per_single_read);
    SLOW5_LOG_DEBUG("Parsed %d recs\n",num_read);

    *read = db->slow5_rec;

    slow5_free_db_tmp(db);
    slow5_free_db(db);
    slow5_free_core(core);

    return num_read;
}


int slow5_write_batch(slow5_rec_t **read, slow5_file_t *s5p, int batch_size, int num_threads){

    slow5_core_t *core = slow5_init_core(s5p,batch_size,num_threads);
    slow5_db_t* db = slow5_init_db(core);

    db->n_rec = batch_size;
    free(db->slow5_rec); //stupid lazy for now
    db->slow5_rec = read;
    slow5_work_db(core,db,slow5_work_per_single_read3);
    SLOW5_LOG_DEBUG("Processed %d recs\n",batch_size);

    int num_wr=slow5_write_db(core,db);
    SLOW5_LOG_DEBUG("Written %d recs\n",num_wr);

    db->slow5_rec = NULL;
    slow5_free_db_tmp(db);
    slow5_free_db(db);
    slow5_free_core(core);

    return num_wr;
}



void slow5_free_batch(slow5_rec_t ***read, int num_rec){

    slow5_rec_t **reads = *read;
    for(int i=0;i<num_rec;i++){
        slow5_rec_free(reads[i]);
    }

    free(reads);
    *read = NULL;
}

#ifdef PYSLOW5_DEBUG_THREAD

#define FILE_PATH "test.blow5" //for reading
#define FILE_PATH_WRITE "test.blow5"
//#define FILE_PATH "/home/jamfer/Data/SK/multi_fast5/s5/FAK40634_d1cc054609fe2c5fcdeac358864f9dc81c8bb793_95.blow5"

int read_func(){

    slow5_file_t *sp = slow5_open(FILE_PATH,"r");
    if(sp==NULL){
        fprintf(stderr,"Error in opening file\n");
        exit(EXIT_FAILURE);
    }
    slow5_rec_t **rec = NULL;
    int ret=0;
    int batch_size = 4096;
    int num_thread = 8;
    while((ret = slow5_get_next_batch(&rec,sp,batch_size,num_thread)) > 0){

        for(int i=0;i<ret;i++){
            uint64_t len_raw_signal = rec[i]->len_raw_signal;
            printf("%s\t%ld\n",rec[i]->read_id,len_raw_signal);
        }
        slow5_free_batch(&rec,ret);

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

    ret = slow5_get_batch(&rec, sp, rid, num_rid, num_thread);
    assert(ret==num_rid);
    for(int i=0;i<ret;i++){
        uint64_t len_raw_signal = rec[i]->len_raw_signal;
        printf("%s\t%ld\n",rec[i]->read_id,len_raw_signal);
    }
    slow5_free_batch(&rec,ret);

    slow5_idx_unload(sp);
    slow5_close(sp);

    return 0;
}


int write_func(){

    slow5_file_t *sf = slow5_open_write(FILE_PATH_WRITE,"w");
    if(sf==NULL){
        fprintf(stderr,"Error in opening file\n");
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
    }
    //end of record setup

    ret = slow5_write_batch(rec,sf,batch_size,num_thread);

    if(ret<batch_size){
        fprintf(stderr,"Writing failed\n");
        exit(EXIT_FAILURE);
    }

    slow5_close_write(sf);

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

//gcc -Wall python/slow5threads.c python/slow5_write.c -I include/ lib/libslow5.a  -lpthread -lz -DPYSLOW5_DEBUG_THREAD=1 -O2 -g

#endif