#include "unit_test.h"
#include <slow5/slow5.h>
#include "slow5_extra.h"
#include "slow5_misc.h"

int slow5_open_valid(void) {
    struct slow5_file *s5p = slow5_open("test/data/exp/aux_array/exp_lossless_end_reason.slow5", "r");
    ASSERT(s5p);
    ASSERT(slow5_idx_load(s5p) == 0);

    struct slow5_rec *read = NULL;
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", &read, s5p) == 0);

    int err;
    uint8_t end_reason = slow5_aux_get_enum(read, "end_reason", &err); /* TODO call this slow5_read_get_enum */
    ASSERT(end_reason == 4); // signal_positive
    ASSERT(err == 0);

    uint8_t n;
    const char **end_reason_labels = slow5_get_aux_enum_labels(s5p->header, "end_reason", &n); /* TODO this api naming is horrible */
    ASSERT(end_reason_labels);
    ASSERT(n == 6);
    ASSERT(strcmp(end_reason_labels[end_reason], "signal_positive") == 0);

    for (int i = 0; i < n; ++ i) {
        ASSERT(printf("%s,", end_reason_labels[i]) == strlen(end_reason_labels[i]) + 1);
    }
    ASSERT(printf("\n") == 1);

    /* TODO make something like this
     * struct slow5_rec_aux_data end_reason_data = slow5_rec_get_aux_data(read, s5p->header->aux_meta, "end_reason");
     * instead of the following
     * freeing end_reason data before setting */
    khint_t pos = kh_get(slow5_s2a, read->aux_map, "end_reason");
    struct slow5_rec_aux_data end_reason_data = kh_val(read->aux_map, pos);
    free(end_reason_data.data);

    end_reason = 0;
    ASSERT(slow5_rec_set(read, s5p->header->aux_meta, "end_reason", &end_reason) == 0);
    end_reason = slow5_aux_get_enum(read, "end_reason", &err);
    ASSERT(end_reason == 0); // unknown
    ASSERT(err == 0);
    ASSERT(strcmp(end_reason_labels[end_reason], "unknown") == 0);

    end_reason = 100;
    ASSERT(slow5_rec_set(read, s5p->header->aux_meta, "end_reason", &end_reason) == -4);

    free(end_reason_labels);
    slow5_rec_free(read);
    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int slow5_get_enum_labels_invalid(void) {

    struct slow5_file *s5p = slow5_open("test/data/exp/aux_array/exp_lossless_end_reason.slow5", "r");
    ASSERT(s5p);

    uint8_t n;
    const char **end_reason_labels;

    /* SLOW5_ERR_ARG */
    end_reason_labels = slow5_get_aux_enum_labels(NULL, "end_reason", &n);
    ASSERT(!end_reason_labels);
    ASSERT(slow5_errno = SLOW5_ERR_ARG);
    end_reason_labels = slow5_get_aux_enum_labels(s5p->header, NULL, &n);
    ASSERT(!end_reason_labels);
    ASSERT(slow5_errno = SLOW5_ERR_ARG);
    end_reason_labels = slow5_get_aux_enum_labels(s5p->header, "end_reason", NULL);
    ASSERT(end_reason_labels);
    ASSERT(strcmp(end_reason_labels[4], "signal_positive") == 0);
    free(end_reason_labels);

    /* SLOW5_ERR_TYPE */
    end_reason_labels = slow5_get_aux_enum_labels(s5p->header, "test_array", &n);
    ASSERT(!end_reason_labels);
    ASSERT(slow5_errno = SLOW5_ERR_TYPE);

    /* SLOW5_ERR_FLD */
    end_reason_labels = slow5_get_aux_enum_labels(s5p->header, "whatisthis", &n);
    ASSERT(!end_reason_labels);
    ASSERT(slow5_errno = SLOW5_ERR_NOFLD);

    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int slow5_get_enum_labels_invalid_noaux(void) {

    struct slow5_file *s5p = slow5_open("test/data/exp/two_rg/exp_default.slow5", "r");
    ASSERT(s5p);

    uint8_t n;
    const char **end_reason_labels;

    /* SLOW5_ERR_NOAUX */
    end_reason_labels = slow5_get_aux_enum_labels(s5p->header, "end_reason", &n);
    ASSERT(!end_reason_labels);
    ASSERT(slow5_errno = SLOW5_ERR_NOAUX);

    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int slow5_get_enum_labels_invalid_noenum(void) {

    struct slow5_file *s5p = slow5_open("test/data/exp/aux_array/exp_lossless.slow5", "r");
    ASSERT(s5p);

    uint8_t n;
    const char **end_reason_labels;

    /* SLOW5_ERR_TYPE */
    end_reason_labels = slow5_get_aux_enum_labels(s5p->header, "end_reason", &n);
    ASSERT(!end_reason_labels);
    ASSERT(slow5_errno = SLOW5_ERR_ARG);

    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int enum_parse_valid(void) {

    char valid_enum1[] = "enum{label}";
    char *valid_enum1_labels[] = { "label" };

    char valid_enum2[] = "enum{label1,label2}";
    char *valid_enum2_labels[] = { "label1", "label2" };

    char valid_enum3[] = "enum{x,y,z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w}";
    char *valid_enum3_labels[] = {"x","y","z","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w"};

    char valid_enum4[] = "enum*{label}";
    char *valid_enum4_labels[] = { "label" };

    char valid_enum5[] = "enum*{label1,label2}";
    char *valid_enum5_labels[] = { "label1", "label2" };

    char valid_enum6[] = "enum*{x,y,z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w}";
    char *valid_enum6_labels[] = {"x","y","z","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w"};

    uint8_t n;
    char **labels = slow5_aux_meta_enum_parse(valid_enum1, SLOW5_ENUM, &n);
    ASSERT(labels);
    ASSERT(n == SLOW5_LENGTH(valid_enum1_labels));
    for (uint16_t i = 0; i < n; ++ i) {
        ASSERT(strcmp(labels[i], valid_enum1_labels[i]) == 0);
        free(labels[i]);
    }
    free(labels);

    labels = slow5_aux_meta_enum_parse(valid_enum2, SLOW5_ENUM, &n);
    ASSERT(labels);
    ASSERT(n == SLOW5_LENGTH(valid_enum2_labels));
    for (uint16_t i = 0; i < n; ++ i) {
        ASSERT(strcmp(labels[i], valid_enum2_labels[i]) == 0);
        free(labels[i]);
    }
    free(labels);

    labels = slow5_aux_meta_enum_parse(valid_enum3, SLOW5_ENUM, &n);
    ASSERT(labels);
    ASSERT(n == SLOW5_LENGTH(valid_enum3_labels));
    for (uint16_t i = 0; i < n; ++ i) {
        ASSERT(strcmp(labels[i], valid_enum3_labels[i]) == 0);
        free(labels[i]);
    }
    free(labels);

    labels = slow5_aux_meta_enum_parse(valid_enum4, SLOW5_ENUM_ARRAY, &n);
    ASSERT(labels);
    ASSERT(n == SLOW5_LENGTH(valid_enum4_labels));
    for (uint16_t i = 0; i < n; ++ i) {
        ASSERT(strcmp(labels[i], valid_enum4_labels[i]) == 0);
        free(labels[i]);
    }
    free(labels);

    labels = slow5_aux_meta_enum_parse(valid_enum5, SLOW5_ENUM_ARRAY, &n);
    ASSERT(labels);
    ASSERT(n == SLOW5_LENGTH(valid_enum5_labels));
    for (uint16_t i = 0; i < n; ++ i) {
        ASSERT(strcmp(labels[i], valid_enum5_labels[i]) == 0);
        free(labels[i]);
    }
    free(labels);

    labels = slow5_aux_meta_enum_parse(valid_enum6, SLOW5_ENUM_ARRAY, &n);
    ASSERT(labels);
    ASSERT(n == SLOW5_LENGTH(valid_enum6_labels));
    for (uint16_t i = 0; i < n; ++ i) {
        ASSERT(strcmp(labels[i], valid_enum6_labels[i]) == 0);
        free(labels[i]);
    }
    free(labels);

    return EXIT_SUCCESS;
}

int enum_parse_invalid(void) {

    char invalid_enum1[] = "enum";
    char invalid_enum2[] = "enum{";
    char invalid_enum3[] = "enum}";
    char invalid_enum4[] = "enum*";
    char invalid_enum5[] = "enum{}";
    char invalid_enum6[] = "enum{a,}";
    char invalid_enum7[] = "enum{,}";
    char invalid_enum8[] = "enum{,,,}";
    char invalid_enum9[] = "enum{*}";
    char invalid_enum10[] = "enum{abcd, efg}";
    char invalid_enum11[] = "enum{123,}";
    char invalid_enum12[] = "enum{abc,efg ,hi}";
    char invalid_enum13[] = "enum{ x}";
    char invalid_enum14[] = "enum{x }";
    char invalid_enum15[] = "enum{x   }";
    char invalid_enum16[] = "enum{   x   }";
    char invalid_enum17[] = "enum{   x}";
    char invalid_enum18[] = "enum{abc;efg;hij}";
    char invalid_enum19[] = "enum(abc,efg,hij)";
    char invalid_enum20[] = "enum{123}";

    uint8_t n;
    char **labels = slow5_aux_meta_enum_parse(invalid_enum1, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum2, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum3, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum4, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum4, SLOW5_ENUM_ARRAY, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum5, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum6, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum7, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum8, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum9, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum10, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum11, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum12, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum13, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum14, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum15, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum16, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum17, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum18, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum19, SLOW5_ENUM, &n);
    ASSERT(!labels);
    labels = slow5_aux_meta_enum_parse(invalid_enum20, SLOW5_ENUM, &n);
    ASSERT(!labels);

    return EXIT_SUCCESS;
}

int slow5_enum_parse_duplicate(void) {

    char invalid_enum1[] = "enum{a,a}";

    uint8_t n;
    char **labels = slow5_aux_meta_enum_parse(invalid_enum1, SLOW5_ENUM, &n);
    ASSERT(!labels);

    struct slow5_file *s5p = slow5_open("test/data/exp/aux_array/exp_lossless_end_reason_dup.slow5", "r");
    ASSERT(!s5p);
    ASSERT(slow5_errno == SLOW5_ERR_HDRPARSE);

    return EXIT_SUCCESS;
}

int slow5_get_set_enum_array(void) {

    struct slow5_file *s5p = slow5_open("test/data/exp/aux_array/exp_lossless_end_reason.slow5", "r");
    ASSERT(s5p);
    ASSERT(slow5_idx_load(s5p) == 0);

    uint8_t enum_array_valid[] = { 0, 1, 2, 3, 4, 5 };
    uint8_t enum_array_invalid[] = { 0, 1, 2, 3, 4, 5, 6 };
    const char *enum_labels[] = {"unk","par","mux","unblock","plus","neg"};
    const char *enum_labels_bad[] = {"unk","par","mux","unblock","+","-"};

    struct slow5_rec *read = NULL;
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", &read, s5p) == 0);

    ASSERT(slow5_aux_meta_add_enum(s5p->header->aux_meta, "end_reasons", SLOW5_ENUM_ARRAY, enum_labels_bad, SLOW5_LENGTH(enum_labels_bad)) == -4);
    ASSERT(slow5_aux_meta_add_enum(s5p->header->aux_meta, "end_reasons", SLOW5_ENUM_ARRAY, enum_labels, SLOW5_LENGTH(enum_labels)) == 0);

    ASSERT(slow5_rec_set_array(read, s5p->header->aux_meta, "end_reasons", enum_array_valid, SLOW5_LENGTH(enum_array_valid)) == 0);
    slow5_hdr_print(s5p->header, SLOW5_FORMAT_ASCII, SLOW5_COMPRESS_NONE);
    slow5_rec_print(read, s5p->header->aux_meta, SLOW5_FORMAT_ASCII, NULL);

    ASSERT(slow5_rec_set_array(read, s5p->header->aux_meta, "end_reasons", enum_array_invalid, SLOW5_LENGTH(enum_array_invalid)) == -4);

    slow5_rec_free(read);
    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int main(void) {

    slow5_set_log_level(SLOW5_LOG_OFF);
    slow5_set_exit_condition(SLOW5_EXIT_OFF);

    struct command tests[] = {
        CMD(slow5_open_valid)
        CMD(slow5_get_enum_labels_invalid)
        CMD(slow5_get_enum_labels_invalid_noaux)
        CMD(slow5_get_enum_labels_invalid_noenum)
        CMD(enum_parse_valid)
        CMD(enum_parse_invalid)
        CMD(slow5_enum_parse_duplicate)
        CMD(slow5_get_set_enum_array)
    };

    return RUN_TESTS(tests);
}
