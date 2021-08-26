#include "unit_test.h"
#include <slow5/slow5.h>

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

int main(void) {

    slow5_set_log_level(SLOW5_LOG_OFF);
    slow5_set_exit_condition(SLOW5_EXIT_OFF);

    struct command tests[] = {
        CMD(slow5_open_valid)
        CMD(slow5_get_enum_labels_invalid)
        CMD(slow5_get_enum_labels_invalid_noaux)
        CMD(slow5_get_enum_labels_invalid_noenum)
    };

    return RUN_TESTS(tests);
}
