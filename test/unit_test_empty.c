#include "unit_test.h"
#include <slow5/slow5.h>
#include "slow5_extra.h"

int slow5_hdr_init_empty_valid(void) {
    struct slow5_hdr *header = slow5_hdr_init_empty();
    slow5_hdr_free(header);

    return EXIT_SUCCESS;
}

int slow5_hdr_add(void) {
    struct slow5_hdr *header = slow5_hdr_init_empty();
    header->version = SLOW5_VERSION_STRUCT;
    ASSERT(slow5_hdr_add_rg(header) == 0);
    ASSERT(slow5_hdr_add_rg(header) == 1);
    ASSERT(slow5_hdr_add_rg(header) == 2);
    ASSERT(slow5_hdr_add_attr("lol", header) == 0);
    ASSERT(slow5_hdr_add_attr("lol", header) == -2);
    ASSERT(slow5_hdr_print(header, SLOW5_FORMAT_ASCII, SLOW5_COMPRESS_NONE, SLOW5_COMPRESS_NONE) != -1);
    slow5_hdr_free(header);

    return EXIT_SUCCESS;
}

int slow5_hdr_add_set(void) {
    struct slow5_hdr *header = slow5_hdr_init_empty();

    ASSERT(slow5_hdr_add_rg(header) == 0);
    ASSERT(slow5_hdr_add_rg(header) == 1);
    ASSERT(slow5_hdr_add_rg(header) == 2);
    ASSERT(slow5_hdr_add_attr("lol", header) == 0);
    ASSERT(slow5_hdr_add_attr("lol", header) == -2);
    ASSERT(slow5_hdr_set("lol", "haha", 2, header) == 0);
    ASSERT(slow5_hdr_set("lol", "good meme", 1, header) == 0);
    ASSERT(slow5_hdr_set("lol", "haha 2", 2, header) == 0);

    ASSERT(slow5_hdr_print(header, SLOW5_FORMAT_ASCII, SLOW5_COMPRESS_NONE, SLOW5_COMPRESS_NONE) != -1);
    slow5_hdr_free(header);

    return EXIT_SUCCESS;
}

int slow5_rec_init_empty_valid(void) {
    struct slow5_rec *read = slow5_rec_init();
    slow5_rec_free(read);

    return EXIT_SUCCESS;
}


int slow5_rec_set_valid(void) {
    struct slow5_rec *read = slow5_rec_init();

    struct slow5_aux_meta *aux_meta = slow5_aux_meta_init_empty();
    //ASSERT(SLOW5_AUX_META_ADD(aux_meta, "channel_number", char*) == 0);
    ASSERT(slow5_aux_meta_add(aux_meta, "channel_number", SLOW5_STRING) == 0);
    ASSERT(slow5_aux_meta_add(aux_meta, "median_before", SLOW5_DOUBLE) == 0);
    //ASSERT(SLOW5_AUX_META_ADD(aux_meta, "read_number", int32_t) == 0);
    ASSERT(slow5_aux_meta_add(aux_meta, "read_number", SLOW5_INT32_T) == 0);
    ASSERT(slow5_aux_meta_add(aux_meta, "start_mux", SLOW5_UINT8_T) == 0);
    ASSERT(slow5_aux_meta_add(aux_meta, "start_time", SLOW5_UINT64_T) == 0);
    ASSERT(slow5_aux_meta_add(aux_meta, "some_char", SLOW5_CHAR) == 0);

    ASSERT(strcmp(aux_meta->attrs[0], "channel_number") == 0);
    ASSERT(strcmp(aux_meta->attrs[1], "median_before") == 0);
    ASSERT(strcmp(aux_meta->attrs[2], "read_number") == 0);

    ASSERT(aux_meta->types[0] == SLOW5_STRING);
    ASSERT(aux_meta->types[1] == SLOW5_DOUBLE);
    ASSERT(aux_meta->types[2] == SLOW5_INT32_T);

    ASSERT(aux_meta->num == 6);

    struct slow5_hdr *header = slow5_hdr_init_empty();
    header->version = SLOW5_VERSION_STRUCT;
    header->aux_meta = aux_meta;
    header->num_read_groups = 1;
    ASSERT(slow5_hdr_print(header, SLOW5_FORMAT_ASCII, SLOW5_COMPRESS_NONE, SLOW5_COMPRESS_NONE) != -1);

    char *cn = "1010";
    double mb = 225.69;
    int32_t rn = 292;
    uint8_t sm = 1;
    uint64_t st = 1019283;
    char x = '\0';

    //ASSERT(slow5_rec_set_array(read, aux_meta, "channel_number", cn, strlen(cn) + 1) == 0);
    ASSERT(slow5_rec_set_string(read, aux_meta, "channel_number", cn) == 0);
    ASSERT(slow5_rec_set(read, aux_meta, "start_time", &st) == 0);
    ASSERT(slow5_rec_set(read, aux_meta, "read_number", &rn) == 0);
    ASSERT(slow5_rec_set(read, aux_meta, "median_before", &mb) == 0);
    ASSERT(slow5_rec_set(read, aux_meta, "start_mux", &sm) == 0);
    ASSERT(slow5_rec_set(read, aux_meta, "some_char", &x) == 0);

    ASSERT(slow5_rec_print(read, aux_meta, SLOW5_FORMAT_ASCII, NULL) != -1);
    slow5_rec_free(read);
    slow5_hdr_free(header);

    return EXIT_SUCCESS;
}


int main(void) {

    struct command tests[] = {
        CMD(slow5_hdr_init_empty_valid)

        CMD(slow5_hdr_add)
        CMD(slow5_hdr_add_set)

        CMD(slow5_rec_init_empty_valid)
        CMD(slow5_rec_set_valid)
    };

    return RUN_TESTS(tests);
}
