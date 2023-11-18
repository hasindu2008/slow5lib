//loads a batch of reads (signal+information needed for pA conversion) from file, process the batch (convert to pA and sum), and write output
//only the time for loading a batch is measured
//make zstd=1
//gcc -Wall -O2 -g -I include/ -o convert_to_pa test/bench/convert_to_pa.c lib/libslow5.a  -lm -lz -lzstd -fopenmp
//only the time for loading a batch to memory (Disk I/O + decompression + parsing and filling the memory arrays) is measured

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>
#include <omp.h>
#include <sys/time.h>
#include "../src/slow5_extra.h"

static inline double realtime(void) {
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return tp.tv_sec + tp.tv_usec * 1e-6;
}



/* load a data batch from disk */
int load_raw_batch(char ***mem_records_a, size_t **mem_bytes_a, slow5_file_t *sf, int batch_size) {

    char **mem_records = (char **)malloc(batch_size * sizeof(char *));
    size_t *mem_bytes = (size_t *)malloc(batch_size * sizeof(size_t));


    int32_t i = 0;
    while (i < batch_size) {
        mem_records[i] = (char *)slow5_get_next_mem(&(mem_bytes[i]), sf);

        if (mem_records[i] == NULL) {
            if (slow5_errno != SLOW5_ERR_EOF) {
                fprintf(stderr,"Error reading from SLOW5 file %d\n", slow5_errno);
                exit(EXIT_FAILURE);
            }
            else {
                break;
            }
        }
        else {
            i++;
        }
    }

    *mem_records_a = mem_records;
    *mem_bytes_a = mem_bytes;

    return i;
}

void free_raw_batch(char **mem_records, size_t *mem_bytes, int batch_size) {
    for(int i=0;i<batch_size;i++) {
        free(mem_records[i]);
    }
    free(mem_records);
    free(mem_bytes);

}


int main(int argc, char *argv[]) {

    if(argc != 4) {
        fprintf(stderr, "Usage: %s reads.blow5 num_thread batch_size\n", argv[0]);
        return EXIT_FAILURE;
    }

    int batch_size = atoi(argv[3]);
    int num_thread = atoi(argv[2]);
    int ret=batch_size;
    omp_set_num_threads(num_thread);

    int read_count = 0;

    double *sums = malloc(sizeof(uint64_t)*batch_size);
    double tot_time = 0;
    double disc_time = 0;
    double t0 = 0;
    double t1 = 0;

    /**** Initialisation and opening of the file ***/
    t0 = realtime();

    slow5_file_t *sp = slow5_open(argv[1],"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       perror("perr: ");
       exit(EXIT_FAILURE);
    }

    tot_time += realtime() - t0;
    /**** End of init ***/

    while(ret == batch_size){

        /**** Fetching a batch (disk loading, decompression, parsing in to memory arrays) ***/
        t0 = realtime();
        char **mem_records = NULL;
        size_t *mem_bytes = NULL;
        ret = load_raw_batch(&mem_records, &mem_bytes, sp, batch_size);
        t1= realtime() - t0;
        tot_time += t1;
        disc_time += t1;

        t0 = realtime();
        read_count += ret;
        slow5_rec_t **rec = (slow5_rec_t**)calloc(batch_size,sizeof(slow5_rec_t*));
        #pragma omp parallel for
        for(int i=0;i<ret;i++){
            if(slow5_rec_depress_parse(&mem_records[i], &mem_bytes[i], NULL, &rec[i], sp)!=0){
                fprintf(stderr,"Error parsing the record %s",rec[i]->read_id);
                exit(EXIT_FAILURE);
            }
        }
        tot_time += realtime() - t0;
        /**** Batch fetched ***/

        fprintf(stderr,"batch loaded with %d reads\n",ret);

        //process and print (time not measured as we want to compare to the time it takes to read the file)
        #pragma omp parallel for
        for(int i=0;i<ret;i++){
            uint64_t sum = 0;
            double scale = rec[i]->range / rec[i]->digitisation;
            for(int j=0; j<rec[i]->len_raw_signal; j++){
                sum += ((rec[i]->raw_signal[j] + rec[i]->offset) * scale);
            }
            sums[i] = sum;
        }
        fprintf(stderr,"batch processed with %d reads\n",ret);
        for(int i=0;i<ret;i++){
            fprintf(stdout,"%s\t%f\n",rec[i]->read_id,sums[i]);
        }
        fprintf(stderr,"batch printed with %d reads\n",ret);

        /**** Deinit ***/
        t0 = realtime();
        free_raw_batch(mem_records, mem_bytes, ret);
        for(int i=0;i<ret;i++){
            slow5_rec_free(rec[i]);
        }
        free(rec);
        tot_time += realtime() - t0;
        /**** End of Deinit***/

    }

    /**** Deinit ***/
    t0 = realtime();
    slow5_close(sp);
    tot_time += realtime() - t0;
    /**** End of Deinit***/

    free(sums);

    fprintf(stderr,"Reads: %d\n",read_count);
    fprintf(stderr,"Time for disc reading %f\n",disc_time);
    fprintf(stderr,"Time for getting samples (disc+depress+parse) %f\n", tot_time);

    return 0;
}
