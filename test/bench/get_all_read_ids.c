//get all the read ids and print them to stdout
//make zstd=1
//gcc -Wall -O2 -I include/ -o get_all_read_ids test/bench/get_all_read_ids.c lib/libslow5.a -lm -lz -lzstd

#include <stdio.h>
#include <stdlib.h>
#include <slow5/slow5.h>
#include <sys/time.h>

static inline double realtime(void) {
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return tp.tv_sec + tp.tv_usec * 1e-6;
}


int main(int argc, char *argv[]) {

    if(argc != 3) {
        fprintf(stderr, "Usage: %s in_file.blow5 out_file.csv\n", argv[0]);
        return EXIT_FAILURE;
    }

    double tot_time = 0;
    double t0 = realtime();

    slow5_file_t *sp = slow5_open(argv[1],"r");
    if(sp==NULL){
       fprintf(stderr,"Error in opening file\n");
       exit(EXIT_FAILURE);
    }

    int ret=0;
    ret = slow5_idx_load(sp);
    if(ret<0){
        fprintf(stderr,"Error in loading index\n");
        exit(EXIT_FAILURE);
    }

    uint64_t num_reads = 0;
    char **read_ids = slow5_get_rids(sp, &num_reads);

    tot_time += realtime() - t0;


    FILE *fp = fopen(argv[2],"w");
    if(fp==NULL){
        fprintf(stderr,"Error in opening file %s for writing\n",argv[2]);
        exit(EXIT_FAILURE);
    }

    fputs("read_id\n", fp);
    for(int i=0; i<num_reads; i++) {
        fputs(read_ids[i],fp);
        fputc('\n',fp);
    }
    fclose(fp);

    t0 = realtime();
    slow5_idx_unload(sp);
    slow5_close(sp);
    tot_time += realtime() - t0;

    fprintf(stderr,"Time taken for getting read IDs: %f\n", tot_time);

}
