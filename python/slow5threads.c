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

#define WORK_STEAL 1 //simple work stealing enabled or not (no work stealing mean no load balancing)
#define STEAL_THRESH 1 //stealing threshold

#define MALLOC_CHK(ret) malloc_chk((void*)ret, __func__, __FILE__, __LINE__ - 1)
#define NEG_CHK(ret) neg_chk(ret, __func__, __FILE__, __LINE__ - 1)

static inline void malloc_chk(void* ret, const char* func, const char* file,
                              int line) {
    if (ret != NULL)
        return;
    fprintf(
        stderr,
        "[%s::ERROR]\033[1;31m Failed to allocate memory : "
        "%s.\033[0m\n[%s::DEBUG]\033[1;35m Error occured at %s:%d. Try with a small batchsize (-K and/or -B options),"
        "fewer threads (-t) or skip ultra-long reads (--skip-ultra) to reduce the peak memory."
        "See https://f5c.page.link/troubleshoot for details.\033[0m\n\n",
        func, strerror(errno), func, file, line);
    exit(EXIT_FAILURE);
}

// Die on error. Print the error and exit if the return value of the previous function is -1
static inline void neg_chk(int ret, const char* func, const char* file,
                           int line) {
    if (ret >= 0)
        return;
    fprintf(stderr,
            "[%s::ERROR]\033[1;31m %s.\033[0m\n[%s::DEBUG]\033[1;35m Error "
            "occured at %s:%d.\033[0m\n\n",
            func, strerror(errno), func, file, line);
    exit(EXIT_FAILURE);
}


/* a batch of read data (dynamic data based on the reads) */
typedef struct {
    int32_t n_rec;
    int32_t capacity_rec;

    char **mem_records; //unused in get()
    size_t *mem_bytes;

    slow5_rec_t **slow5_rec;
    char **rid; //only used in get()

} db_t;



/* core data structure (mostly static data throughout the program lifetime) */
typedef struct {
    //slow5
    slow5_file_t *sf;
    int num_thread;
    int batch_size;
} core_t;


/* argument wrapper for the multithreaded framework used for data processing */
typedef struct {
    core_t* core;
    db_t* db;
    int32_t starti;
    int32_t endi;
    void (*func)(core_t*,db_t*,int);
    int32_t thread_index;
#ifdef WORK_STEAL
    void *all_pthread_args;
#endif
#ifdef HAVE_CUDA
    int32_t *ultra_long_reads; //reads that are assigned to the CPU due to the unsuitability to process on the GPU
    double ret1;    //return value
#endif
} pthread_arg_t;


/* initialise the core data structure */
core_t* init_core(slow5_file_t *s5p, int batch_size, int num_thread) {

    core_t* core = (core_t*)malloc(sizeof(core_t));
    MALLOC_CHK(core);

    core->sf = s5p;
    core->batch_size = batch_size;
    core->num_thread = num_thread;

    return core;
}

/* free the core data structure */
void free_core(core_t* core) {
    free(core);
}

/* initialise a data batch */
db_t* init_db(core_t* core) {
    db_t* db = (db_t*)(malloc(sizeof(db_t)));
    MALLOC_CHK(db);

    db->capacity_rec = core->batch_size;
    db->n_rec = 0;

    db->mem_records = (char**)(calloc(db->capacity_rec,sizeof(char*)));
    MALLOC_CHK(db->mem_records);
    db->mem_bytes = (size_t*)(calloc(db->capacity_rec,sizeof(size_t)));
    MALLOC_CHK(db->mem_bytes);

    db->slow5_rec = (slow5_rec_t**)calloc(db->capacity_rec,sizeof(slow5_rec_t*));
    MALLOC_CHK(db->slow5_rec);

    return db;
}

/* load a data batch from disk */
int load_db(core_t* core, db_t* db) {

    db->n_rec = 0;

    int32_t i = 0;
    while (db->n_rec < db->capacity_rec) {
        i=db->n_rec;
        db->mem_records[i] = (char *)slow5_get_next_mem(&(db->mem_bytes[i]), core->sf);

        if (db->mem_records[i] == NULL) {
            if (slow5_errno != SLOW5_ERR_EOF) {
                fprintf(stderr,"Error reading from SLOW5 file %d\n", slow5_errno);
                exit(EXIT_FAILURE);
            }
            else {
                fprintf(stderr,"Last Batch!\n");
                break;
            }
        }
        else {
            db->n_rec++;
        }
    }

    return db->n_rec;
}


void parse_single(core_t* core,db_t* db, int32_t i){

    assert(db->mem_bytes[i]>0);
    assert(db->mem_records[i]!=NULL);
    int ret=slow5_rec_depress_parse(&db->mem_records[i], &db->mem_bytes[i], NULL, &db->slow5_rec[i], core->sf);
    if(ret!=0){
        fprintf(stderr,"Error parsing the record %d",i);
        exit(EXIT_FAILURE);
    }

}


void work_per_single_read(core_t* core,db_t* db, int32_t i){
    parse_single(core,db,i);
}

void work_per_single_read2(core_t* core,db_t* db, int32_t i){
    assert(db->rid[i]!=NULL);
    int ret = slow5_get(db->rid[i],&db->slow5_rec[i], core->sf);
    if(ret<0){
        fprintf(stderr,"Error when fetching the read %s\n",db->rid[i]);
        exit(EXIT_FAILURE);
    }
    db->mem_bytes[i]=ret;

}

/* partially free a data batch - only the read dependent allocations are freed */
void free_db_tmp(db_t* db) {
    int32_t i = 0;
    for (i = 0; i < db->n_rec; ++i) {
        free(db->mem_records[i]);
    }
}

/* completely free a data batch */
void free_db(db_t* db) {

    free(db->mem_records);
    free(db->mem_bytes);;

    free(db);
}


static inline int32_t steal_work(pthread_arg_t* all_args, int32_t num_thread) {
	int32_t i, c_i = -1;
	int32_t k;
	for (i = 0; i < num_thread; ++i){
        pthread_arg_t args = all_args[i];
        //fprintf(stderr,"endi : %d, starti : %d\n",args.endi,args.starti);
		if (args.endi-args.starti > STEAL_THRESH) {
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


void* pthread_single(void* voidargs) {
    int32_t i;
    pthread_arg_t* args = (pthread_arg_t*)voidargs;
    db_t* db = args->db;
    core_t* core = args->core;

#ifndef WORK_STEAL
    for (i = args->starti; i < args->endi; i++) {
        args->func(core,db,i);
    }
#else
    pthread_arg_t* all_args = (pthread_arg_t*)(args->all_pthread_args);
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

void pthread_db(core_t* core, db_t* db, void (*func)(core_t*,db_t*,int)){
    //create threads
    pthread_t tids[core->num_thread];
    pthread_arg_t pt_args[core->num_thread];
    int32_t t, ret;
    int32_t i = 0;
    int32_t num_thread = core->num_thread;
    int32_t step = (db->n_rec + num_thread - 1) / num_thread;
    //todo : check for higher num of threads than the data
    //current works but many threads are created despite

    fprintf(stderr,"Creating %d threads\n",num_thread);
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
    #ifdef WORK_STEAL
        pt_args[t].all_pthread_args =  (void *)pt_args;
    #endif
        //fprintf(stderr,"t%d : %d-%d\n",t,pt_args[t].starti,pt_args[t].endi);

    }

    //create threads
    for(t = 0; t < core->num_thread; t++){
        ret = pthread_create(&tids[t], NULL, pthread_single,
                                (void*)(&pt_args[t]));
        NEG_CHK(ret);
    }

    //pthread joining
    for (t = 0; t < core->num_thread; t++) {
        int ret = pthread_join(tids[t], NULL);
        NEG_CHK(ret);
    }
}

/* process all reads in the given batch db */
void work_db(core_t* core, db_t* db, void (*func)(core_t*,db_t*,int)){

    if (core->num_thread == 1) {
        int32_t i=0;
        for (i = 0; i < db->n_rec; i++) {
            func(core,db,i);
        }

    }

    else {
        pthread_db(core,db,func);
    }
}

void process_db(core_t* core,db_t* db){
    work_db(core, db, work_per_single_read);
}


int slow5_get_batch(slow5_rec_t ***read, slow5_file_t *s5p, char **rid, int num_rid, int num_threads){

    core_t *core = init_core(s5p,num_rid,num_threads);
    db_t* db = init_db(core);

    db->rid = rid;
    db->n_rec = num_rid;
    work_db(core,db,work_per_single_read2);
    fprintf(stderr,"loaded and parsed %d recs\n",num_rid);

    *read = db->slow5_rec;

    free_db_tmp(db);
    free_db(db);
    free_core(core);

    return num_rid;
}


int slow5_get_next_batch(slow5_rec_t ***read, slow5_file_t *s5p, int batch_size, int num_threads){

    core_t *core = init_core(s5p,batch_size,num_threads);
    db_t* db = init_db(core);

    int num_read=load_db(core,db);
    fprintf(stderr,"Loaded %d recs\n",num_read);
    work_db(core,db,work_per_single_read);
    fprintf(stderr,"Parsed %d recs\n",num_read);

    *read = db->slow5_rec;

    free_db_tmp(db);
    free_db(db);
    free_core(core);

    return num_read;
}


void slow5_free_batch(slow5_rec_t ***read, int num_rec){

    slow5_rec_t **reads = *read;
    for(int i=0;i<num_rec;i++){
        slow5_rec_free(reads[i]);
    }

    free(reads);
    *read = NULL;
}

#if DEBUG

#define FILE_PATH "../f5c/test/chr22_meth_example/reads.blow5"
//#define FILE_PATH "/home/jamfer/Data/SK/multi_fast5/s5/FAK40634_d1cc054609fe2c5fcdeac358864f9dc81c8bb793_95.blow5"

int main(){

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
    rid[0]="428922bd-39fe-4e4b-9baa-478334aa17d0";
    rid[1]="da81525b-b4d5-410b-ab8b-e5dd97aa7420",
    rid[2]="7b12f432-82b8-4170-a578-8e6acbb1f658";
    rid[3]="f6327045-c041-4c9e-8a14-c430e990a3c2";

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
//gcc -Wall python/slow5threads.c -I include/ lib/libslow5.a  -lpthread -lz -g -O2 -DDEBUG=1

#endif
