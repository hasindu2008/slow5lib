#ifndef PRESS_H
#define PRESS_H

#include <zlib.h>
#include <stdio.h>

// Compression methods
enum PressMethod {
    COMPRESS_NONE,
    COMPRESS_GZIP
};


// Gzip related definitions
// To obtain gzip deflation and inflation set windowBits=MAX_WBITS|GZIP_WBITS
// Used in deflateInit2 and inflateInit2 from zlib

#define NUM_MAGIC_BYTES (2)
static const int GZIP_MAGIC_NUM[] = { 0x1f, 0x8b };

#define GZIP_WBITS (16)
#define Z_MEM_DEFAULT (8)
#define Z_OUT_CHUNK (16384) // 2^14

// Gzip stream
struct GzipStream {
    z_stream strm;
    int flush;
};


// Compression streams
union PressStream {
    struct GzipStream *gzip_stream;
};

// Compression object
struct Press {
    enum PressMethod press_method;
    union PressStream *press_stream;
};


// Init and destroy compression stream
struct Press *press_init(enum PressMethod press_method);
void press_destroy(struct Press **press);

// fwrite but with compression
size_t fwrite_press(struct Press *press, const void *ptr, size_t size, size_t nmemb, FILE *stream);
// fwrite but with gzip compression
size_t fwrite_gzip(struct GzipStream *gzip_stream, const void *ptr, size_t size, size_t nmemb, FILE *stream);

// fprintf but with compression
int fprintf_press(struct Press *press, FILE *stream, const char *format, ...);
// vfprintf but with gzip compression
int vfprintf_gzip(struct GzipStream *gzip_stream, FILE *stream, const char *format, va_list ap);

// Write the compression footer on the immediate next call to fprintf_press
void press_footer_next(struct Press *press);

// sprintf and vsprintf but dynamically allocates strp memory
int asprintf(char **strp, const char *fmt, ...);
int vasprintf(char **strp, const char *fmt, va_list ap);

// Decompress a zlib compressed string of size n
// Return it and set n to the new size
unsigned char *z_inflate_buf(const char *comp_str, size_t *n);
// Write zlib compressed ptr to f_out
int z_deflate_write(z_streamp strmp, const void *ptr, uLong size, FILE *f_out, int flush);

#endif