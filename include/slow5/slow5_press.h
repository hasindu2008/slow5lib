#ifndef SLOW5_PRESS_H
#define SLOW5_PRESS_H

#include <zlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
//#include "slow5_misc.h"

#ifdef __cplusplus
extern "C" {
#endif

// Compression methods
enum slow5_press_method {
    SLOW5_COMPRESS_NONE,
    SLOW5_COMPRESS_GZIP
};
typedef uint8_t slow5_press_method_t;

#define SLOW5_Z_MEM_DEFAULT (8)
#define SLOW5_Z_OUT_CHUNK (16384) // 2^14

// Gzip stream
struct slow5_gzip_stream {
    z_stream strm_inflate;
    z_stream strm_deflate;
    int flush;
};


// Compression streams
union slow5_press_stream {
    struct slow5_gzip_stream *gzip;
};

// Compression object
struct slow5_press {
    slow5_press_method_t method;
    union slow5_press_stream *stream;
};
typedef struct slow5_press slow5_press_t;

/* --- Init / free slow5_press structure --- */
struct slow5_press *slow5_press_init(slow5_press_method_t method);
void slow5_press_free(struct slow5_press *comp);

/* --- Compress / decompress a ptr to some memory --- */
void *slow5_ptr_compress(struct slow5_press *comp, const void *ptr, size_t count, size_t *n);
static inline void *slow5_str_compress(struct slow5_press *comp, const char *str, size_t *n) {
    return slow5_ptr_compress(comp, str, strlen(str) + 1, n); // Include '\0'
}
void *slow5_ptr_depress(struct slow5_press *comp, const void *ptr, size_t count, size_t *n);
void *slow5_ptr_depress_multi(slow5_press_method_t method, const void *ptr, size_t count, size_t *n);

/* --- Compress / decompress a ptr to some file --- */
size_t slow5_fwrite_compress(struct slow5_press *comp, const void *ptr, size_t size, size_t nmemb, FILE *fp);
size_t slow5_fwrite_depress(struct slow5_press *comp, const void *ptr, size_t size, size_t nmemb, FILE *fp); // TODO
static inline size_t slow5_print_compress(struct slow5_press *comp, const void *ptr, size_t size, size_t nmemb) {
    return slow5_fwrite_compress(comp, ptr, size, nmemb, stdout);
}
static inline size_t slow5_print_depress(struct slow5_press *comp, const void *ptr, size_t size, size_t nmemb) {
    return slow5_fwrite_depress(comp, ptr, size, nmemb, stdout);
}
static inline size_t slow5_fwrite_str_compress(struct slow5_press *comp, const char *str, FILE *fp) {
    return slow5_fwrite_compress(comp, str, sizeof *str, strlen(str), fp); // Don't include '\0'
}
static inline size_t slow5_print_str_compress(struct slow5_press *comp, const char *str) {
    return slow5_fwrite_str_compress(comp, str, stdout);
}

/* --- Decompress to a ptr from some file --- */
void *slow5_fread_depress(struct slow5_press *comp, size_t count, FILE *fp, size_t *n);
void *slow5_pread_depress(struct slow5_press *comp, int fd, size_t count, off_t offset, size_t *n);
void *slow5_pread_depress_multi(slow5_press_method_t method, int fd, size_t count, off_t offset, size_t *n);

/* --- Compress with format string to some file --- */
int slow5_fprintf_compress(struct slow5_press *comp, FILE *fp, const char *format, ...);
int slow5_printf_compress(struct slow5_press *comp, const char *format, ...);

/* --- Write compression footer on immediate next compression call --- */
void slow5_compress_footer_next(struct slow5_press *comp);

#ifdef __cplusplus
}
#endif

#endif
