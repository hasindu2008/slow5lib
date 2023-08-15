//loads a batch of reads where the read IDs are in genomic coordinate order (signal+information needed for pA conversion) from file, process the batch (convert to pA and sum), and write output
//only the time for loading a batch is measured
//make zstd=1
//gcc -Wall -O2 -g -I include/ -o convert_to_pa_rand test/bench/convert_to_pa_rand.c lib/libslow5.a  -lm -lz -lzstd -fopenmp
//only the time for loading a batch to memory (Disk I/O + decompression + parsing and filling the memory arrays) is measured

// to generate read id list: samtools view reads.sorted.bam | awk '{print $1}' > rid.txt

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>
#include <omp.h>
#include <sys/time.h>

static inline double realtime(void) {
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return tp.tv_sec + tp.tv_usec * 1e-6;
}


int main(int argc, char *argv[]) {

    if(argc != 5) {
        fprintf(stderr, "Usage: %s reads.blow5 rid_list.txt num_thread batch_size\n", argv[0]);
        return EXIT_FAILURE;
    }

    int batch_size = atoi(argv[4]);
    int num_thread = atoi(argv[3]);
    int ret=batch_size;
    omp_set_num_threads(num_thread);

    int read_count = 0;

    double *sums = malloc(sizeof(uint64_t)*batch_size);
    double tot_time = 0;
    double t0 = 0;

    //read id list
    FILE *fpr = fopen(argv[2],"r");
    if(fpr==NULL){
        fprintf(stderr,"Error in opening file %s for reading\n",argv[2]);
        perror("perr: ");;
        exit(EXIT_FAILURE);
    }

    char **rid = malloc(sizeof(char*)*batch_size);
    char tmp[1024];

    /**** Initialisation and opening of the file ***/
    t0 = realtime();

    slow5_file_t *sp = slow5_open(argv[1],"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       perror("perr: ");
       exit(EXIT_FAILURE);
    }

    ret = slow5_idx_load(sp);
    if(ret<0){
        fprintf(stderr,"Error in loading index\n");
        exit(EXIT_FAILURE);
    }

    tot_time += realtime() - t0;
    /**** End of init ***/

    while(1){

        int i=0;
        for(i=0; i<batch_size; i++){
            if (fscanf(fpr,"%s",tmp) < 1) {
                break;
            }
            rid[i] = strdup(tmp);
        }
        if(i==0) break;
        int ret = i;
        read_count += ret;

        /**** Fetching a batch (disk loading, decompression, parsing in to memory arrays) ***/

        t0 = realtime();
        slow5_rec_t **rec = (slow5_rec_t**)calloc(batch_size,sizeof(slow5_rec_t*));
        #pragma omp parallel for
        for(int i=0;i<ret;i++){
            if(slow5_get(rid[i], &rec[i], sp) < 0){
                fprintf(stderr,"Error fetching the read %s",rid[i]);
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
        for(int i=0;i<ret;i++){
            slow5_rec_free(rec[i]);
        }
        free(rec);
        tot_time += realtime() - t0;
        /**** End of Deinit***/

        for(int i=0; i<ret; i++){
            free(rid[i]);
        }

        if(ret<batch_size){ //this indicates nothing left to read
            break;
        }

    }

    /**** Deinit ***/
    t0 = realtime();
    slow5_idx_unload(sp);
    slow5_close(sp);
    tot_time += realtime() - t0;
    /**** End of Deinit***/

    free(sums);
    free(rid);
    fclose(fpr);

    fprintf(stderr,"Reads: %d\n",read_count);
    fprintf(stderr,"Time for getting samples (disc+depress+parse) %f\n", tot_time);

    return 0;
}
