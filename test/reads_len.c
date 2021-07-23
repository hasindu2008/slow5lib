/*
 * print the length of each read in the slow5
 * usage: ./reads_len slow5_file1 slow5_file2 ...
 * compilation: gcc test/reads_len.c -I include/ -o test/reads_len lib/libslow5.a -lz
 */

#include <slow5/slow5.h>

int main(int argc, char **argv) {

    for (int i = 1; i < argc; ++ i) {
        slow5_file_t *file = slow5_open(argv[i], "r");
        slow5_rec_t *read = NULL;
        while (slow5_get_next(&read, file) != SLOW5_ERR_EOF) {
            size_t n;
            void *mem;
            if ((mem = slow5_rec_to_mem(read, file->header->aux_meta, file->format, file->compress, &n))) {
                printf("%zu\n", n);
                free(mem);
            } else {
                printf("error\n");
            }
        }
        slow5_rec_free(read);
        slow5_close(file);
    }

    return 0;
}
