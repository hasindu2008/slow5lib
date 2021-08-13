#include "unit_test.h"
#include <slow5/slow5.h>
#include <string.h>

int press_init_valid(void) {
    struct slow5_press *comp = slow5_press_init(SLOW5_COMPRESS_NONE);
    ASSERT(comp->method == SLOW5_COMPRESS_NONE);
    ASSERT(comp->stream == NULL);
    slow5_press_free(comp);

    comp = slow5_press_init(SLOW5_COMPRESS_ZLIB);
    ASSERT(comp->method == SLOW5_COMPRESS_ZLIB);
    ASSERT(comp->stream != NULL);
    slow5_press_free(comp);

    return EXIT_SUCCESS;
}

int press_buf_valid(void) {
    struct slow5_press *comp = slow5_press_init(SLOW5_COMPRESS_NONE);

    const char *str = "12345";
    size_t size = 0;
    char *str_same = slow5_str_compress(comp, str, &size);
    ASSERT(strcmp(str_same, str) == 0);
    ASSERT(size == strlen(str) + 1);

    slow5_press_free(comp);

    comp = slow5_press_init(SLOW5_COMPRESS_ZLIB);
    size_t size_zlib = 0;
    slow5_compress_footer_next(comp);
    void *str_zlib = slow5_str_compress(comp, str, &size_zlib);
    char *str_copy = slow5_ptr_depress(comp, str_zlib, size_zlib, &size);
    ASSERT(strcmp(str_copy, str) == 0);
    ASSERT(size == strlen(str_copy) + 1);
    ASSERT(size == strlen(str_same) + 1);
    ASSERT(size == strlen(str) + 1);
    ASSERT(size < size_zlib);
    fwrite(str_zlib, size_zlib, 1, stdout); // TESTING

    free(str_zlib);
    free(str_same);
    free(str_copy);
    slow5_press_free(comp);

    return EXIT_SUCCESS;
}

int press_buf_valid2(void) {
    struct slow5_press *comp = slow5_press_init(SLOW5_COMPRESS_NONE);

    const char *str = "1234567890123456789012345678901234567890";
    size_t size = 0;
    char *str_same = slow5_str_compress(comp, str, &size);
    ASSERT(strcmp(str_same, str) == 0);
    ASSERT(size == strlen(str) + 1);

    slow5_press_free(comp);

    comp = slow5_press_init(SLOW5_COMPRESS_ZLIB);
    size_t size_zlib = 0;
    slow5_compress_footer_next(comp);
    void *str_zlib = slow5_str_compress(comp, str, &size_zlib);
    char *str_copy = slow5_ptr_depress(comp, str_zlib, size_zlib, &size);
    ASSERT(strcmp(str_copy, str) == 0);
    ASSERT(size == strlen(str_copy) + 1);
    ASSERT(size == strlen(str_same) + 1);
    ASSERT(size == strlen(str) + 1);
    ASSERT(size > size_zlib);
    fwrite(str_zlib, size_zlib, 1, stdout); // TESTING

    free(str_zlib);
    free(str_same);
    free(str_copy);
    slow5_press_free(comp);

    return EXIT_SUCCESS;
}

int press_print_valid(void) {
    struct slow5_press *comp = slow5_press_init(SLOW5_COMPRESS_ZLIB);

    const char *str = "hello";
    slow5_compress_footer_next(comp);
    slow5_print_str_compress(comp, str);

    slow5_press_free(comp);

    return EXIT_SUCCESS;
}

int press_printf_valid(void) {
    struct slow5_press *comp = slow5_press_init(SLOW5_COMPRESS_ZLIB);

    const char *str = "lol";
    slow5_compress_footer_next(comp);
    slow5_printf_compress(comp, "\n%s\n", str);

    slow5_press_free(comp);

    return EXIT_SUCCESS;
}

int press_svb_one_valid(void) {

    const int16_t one[] = { 100 };
    size_t bytes_svb = 0;
    uint8_t *one_svb = slow5_ptr_compress_solo(SLOW5_COMPRESS_SVB_ZD, one, sizeof *one, &bytes_svb);
    ASSERT(one_svb);
    ASSERT(bytes_svb > 0);

    uint32_t length;
    memcpy(&length, one_svb, sizeof length);
    ASSERT(length == 1);

    size_t bytes_orig = 0;
    int16_t *one_depress = slow5_ptr_depress_solo(SLOW5_COMPRESS_SVB_ZD, one_svb, bytes_svb, &bytes_orig);
    ASSERT(bytes_orig == sizeof *one);
    ASSERT(memcmp(one, one_depress, bytes_orig) == 0);
    ASSERT(one_depress[0] == one[0]);

    free(one_svb);
    free(one_depress);

    return EXIT_SUCCESS;
}

int press_svb_big_valid(void) {

    const int16_t big[] = { 100, -100, 0, 10000, 3442, 234, 2326, 346, 213, 234 };
    size_t bytes_svb = 0;
    uint8_t *big_svb = slow5_ptr_compress_solo(SLOW5_COMPRESS_SVB_ZD, big, sizeof big, &bytes_svb);
    ASSERT(big_svb);
    ASSERT(bytes_svb > 0);

    uint32_t length;
    memcpy(&length, big_svb, sizeof length);
    ASSERT(length == LENGTH(big));

    size_t bytes_orig = 0;
    int16_t *big_depress = slow5_ptr_depress_solo(SLOW5_COMPRESS_SVB_ZD, big_svb, bytes_svb, &bytes_orig);
    ASSERT(bytes_orig == sizeof big);
    ASSERT(memcmp(big, big_depress, bytes_orig) == 0);

    free(big_svb);
    free(big_depress);

    return EXIT_SUCCESS;
}

int press_svb_exp_valid(void) {

    const int16_t big[] = { 1039, 588, 588, 593, 586, 574, 570, 585, 588, 586 };
    size_t bytes_svb = 0;
    uint8_t *big_svb = slow5_ptr_compress_solo(SLOW5_COMPRESS_SVB_ZD, big, sizeof big, &bytes_svb);
    ASSERT(big_svb);
    ASSERT(bytes_svb > 0);
    ASSERT(bytes_svb < sizeof big);

    uint32_t length;
    memcpy(&length, big_svb, sizeof length);
    ASSERT(length == LENGTH(big));

    size_t bytes_orig = 0;
    int16_t *big_depress = slow5_ptr_depress_solo(SLOW5_COMPRESS_SVB_ZD, big_svb, bytes_svb, &bytes_orig);
    ASSERT(bytes_orig == sizeof big);
    ASSERT(memcmp(big, big_depress, bytes_orig) == 0);

    free(big_svb);
    free(big_depress);

    return EXIT_SUCCESS;
}

int press_zstd_buf_valid(void) {

    const char *str = "1234567890123456789012345678901234567890";
    struct slow5_press *comp = slow5_press_init(SLOW5_COMPRESS_ZSTD);

    size_t size_zstd = 0;
    void *str_zstd = slow5_str_compress(comp, str, &size_zstd);
    ASSERT(str_zstd);
    ASSERT(size_zstd < strlen(str));

    size_t size_copy;
    char *str_copy = slow5_ptr_depress(comp, str_zstd, size_zstd, &size_copy);
    ASSERT(strncmp(str_copy, str, strlen(str)) == 0);
    ASSERT(size_copy - 1 == strlen(str));

    free(str_zstd);
    free(str_copy);
    slow5_press_free(comp);

    return EXIT_SUCCESS;
}


int main(void) {

    slow5_set_log_level(SLOW5_LOG_OFF);
    slow5_set_exit_condition(SLOW5_EXIT_OFF);

    struct command tests[] = {
        CMD(press_init_valid)
        CMD(press_buf_valid)
        CMD(press_buf_valid2)
        CMD(press_print_valid)
        CMD(press_printf_valid)

        CMD(press_svb_one_valid)
        CMD(press_svb_big_valid)
        CMD(press_svb_exp_valid)

        CMD(press_zstd_buf_valid)
    };

    return RUN_TESTS(tests);
}
