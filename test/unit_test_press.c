#include "unit_test.h"
#include <slow5/slow5.h>
#include <string.h>

int press_init_valid(void) {
    struct slow5_press *comp = slow5_press_init(SLOW5_COMPRESS_NONE);
    ASSERT(comp->method == SLOW5_COMPRESS_NONE);
    ASSERT(comp->stream == NULL);
    slow5_press_free(comp);

    comp = slow5_press_init(SLOW5_COMPRESS_GZIP);
    ASSERT(comp->method == SLOW5_COMPRESS_GZIP);
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

    comp = slow5_press_init(SLOW5_COMPRESS_GZIP);
    size_t size_gzip = 0;
    slow5_compress_footer_next(comp);
    void *str_gzip = slow5_str_compress(comp, str, &size_gzip);
    char *str_copy = slow5_ptr_depress(comp, str_gzip, size_gzip, &size);
    ASSERT(strcmp(str_copy, str) == 0);
    ASSERT(size == strlen(str_copy) + 1);
    ASSERT(size == strlen(str_same) + 1);
    ASSERT(size == strlen(str) + 1);
    ASSERT(size < size_gzip);
    fwrite(str_gzip, size_gzip, 1, stdout); // TESTING

    free(str_gzip);
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

    comp = slow5_press_init(SLOW5_COMPRESS_GZIP);
    size_t size_gzip = 0;
    slow5_compress_footer_next(comp);
    void *str_gzip = slow5_str_compress(comp, str, &size_gzip);
    char *str_copy = slow5_ptr_depress(comp, str_gzip, size_gzip, &size);
    ASSERT(strcmp(str_copy, str) == 0);
    ASSERT(size == strlen(str_copy) + 1);
    ASSERT(size == strlen(str_same) + 1);
    ASSERT(size == strlen(str) + 1);
    ASSERT(size > size_gzip);
    fwrite(str_gzip, size_gzip, 1, stdout); // TESTING

    free(str_gzip);
    free(str_same);
    free(str_copy);
    slow5_press_free(comp);

    return EXIT_SUCCESS;
}

int press_print_valid(void) {
    struct slow5_press *comp = slow5_press_init(SLOW5_COMPRESS_GZIP);

    const char *str = "hello";
    slow5_compress_footer_next(comp);
    slow5_print_str_compress(comp, str);

    slow5_press_free(comp);

    return EXIT_SUCCESS;
}

int press_printf_valid(void) {
    struct slow5_press *comp = slow5_press_init(SLOW5_COMPRESS_GZIP);

    const char *str = "lol";
    slow5_compress_footer_next(comp);
    slow5_printf_compress(comp, "\n%s\n", str);

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
    };

    return RUN_TESTS(tests);
}
