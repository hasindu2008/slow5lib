#define _XOPEN_SOURCE 700
#include <zlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <slow5/slow5.h>
#include <slow5/slow5_error.h>
#include <slow5/slow5_press.h>
#include <slow5/slow5_defs.h>
#include <streamvbyte.h>
#include <streamvbyte_zigzag.h>
#ifdef SLOW5_USE_ZSTD
#include <zstd.h>
#endif /* SLOW5_USE_ZSTD */
#include "slow5_misc.h"

extern enum slow5_log_level_opt  slow5_log_level;
extern enum slow5_exit_condition_opt  slow5_exit_condition;

/* zlib */
static int zlib_init_deflate(z_stream *strm);
static int zlib_init_inflate(z_stream *strm);
static void *ptr_compress_zlib(struct slow5_zlib_stream *zlib, const void *ptr, size_t count, size_t *n);
static void *ptr_compress_zlib_solo(const void *ptr, size_t count, size_t *n);
static void *ptr_depress_zlib(struct slow5_zlib_stream *zlib, const void *ptr, size_t count, size_t *n);
static void *ptr_depress_zlib_solo(const void *ptr, size_t count, size_t *n);
static ssize_t fwrite_compress_zlib(struct slow5_zlib_stream *zlib, const void *ptr, size_t size, size_t nmemb, FILE *fp);

/* streamvbyte */
static uint8_t *ptr_compress_svb(const uint32_t *ptr, size_t count, size_t *n);
static uint8_t *ptr_compress_svb_zd(const int16_t *ptr, size_t count, size_t *n);
static uint32_t *ptr_depress_svb(const uint8_t *ptr, size_t count, size_t *n);
static int16_t *ptr_depress_svb_zd(const uint8_t *ptr, size_t count, size_t *n);

#ifdef SLOW5_USE_ZSTD
/* zstd */
static void *ptr_compress_zstd(const void *ptr, size_t count, size_t *n);
static void *ptr_depress_zstd(const void *ptr, size_t count, size_t *n);
#endif /* SLOW5_USE_ZSTD */

/* ex_zd */
static uint8_t *ptr_compress_ex_zd(const int16_t *ptr, size_t count, size_t *n);
static int16_t *ptr_depress_ex_zd(const uint8_t *ptr, size_t count, size_t *n);

/* other */
static int vfprintf_compress(struct __slow5_press *comp, FILE *fp, const char *format, va_list ap);


/* Convert the press method back an forth from the library format to spec format
   Implementation is horrible, but for now.
 */

// convert the record compression method from library format to the spec format
uint8_t slow5_encode_record_press(enum slow5_press_method method){
    uint8_t ret = 0;
    switch(method){
        case SLOW5_COMPRESS_NONE:
            ret = 0;
            break;
        case SLOW5_COMPRESS_ZLIB:
            ret = 1;
            break;
        case SLOW5_COMPRESS_ZSTD:
            ret = 2;
            break;
        case SLOW5_COMPRESS_SVB_ZD:  //hidden feature hack for devs
            SLOW5_WARNING("You are using a hidden dev features (record compression in %s). Output files may be useless.","svb-zd");
            ret = 250;
            break;
        default:  //todo: Proper error?
            ret = 255;
            SLOW5_WARNING("Unknown record compression method %d",method);
            break;
    }
    return ret;
}

// convert the record compression method from spec format to the library format
enum slow5_press_method slow5_decode_record_press(uint8_t method){
    enum slow5_press_method ret = SLOW5_COMPRESS_NONE;
    switch(method){
        case 0:
            ret = SLOW5_COMPRESS_NONE;
            break;
        case 1:
            ret = SLOW5_COMPRESS_ZLIB;
            break;
        case 2:
            ret = SLOW5_COMPRESS_ZSTD;
            break;
        case 250:  //hidden feature hack for devs
            ret = SLOW5_COMPRESS_SVB_ZD;
            break;
        default:    //todo Proper error
            ret = 255;
            SLOW5_WARNING("Unknown record compression method %d",method);
            break;
    }
    return ret;
}

// convert the signal compression from library format to the spec format
uint8_t slow5_encode_signal_press(enum slow5_press_method method){
    uint8_t ret = 0;
    switch(method){
        case SLOW5_COMPRESS_NONE:
            ret = 0;
            break;
        case SLOW5_COMPRESS_SVB_ZD:
            ret = 1;
            break;
        case SLOW5_COMPRESS_ZLIB: //hidden feature hack for devs
            SLOW5_WARNING("You are using a hidden dev features (signal compression in %s). Output files may be useless.", "zlib");
            ret = 250;
            break;
        case SLOW5_COMPRESS_ZSTD: //hidden feature hack for devs
            SLOW5_WARNING("You are using a hidden dev features (signal compression in %s). Output files may be useless.","zstd");
            ret = 251;
            break;
        case SLOW5_COMPRESS_EX_ZD: //hidden feature hack for devs
            SLOW5_WARNING("You are using a hidden dev features (signal compression in %s). Output files may be useless.","ex-zd");
            ret = 252;
            break;
        default:    //todo: Proper error?
            ret = 255;
            SLOW5_WARNING("Unknown signal compression method %d",method);
            break;
    }
    return ret;
}

// convert the signal compression from spec format to library format
enum slow5_press_method slow5_decode_signal_press(uint8_t method){
    enum slow5_press_method ret = 0;
    switch(method){
        case 0:
            ret = SLOW5_COMPRESS_NONE;
            break;
        case 1:
            ret = SLOW5_COMPRESS_SVB_ZD;
            break;
        case 250: //hidden feature hack for devs
            ret = SLOW5_COMPRESS_ZLIB;
            break;
        case 251: //hidden feature hack for devs
            ret = SLOW5_COMPRESS_ZSTD;
            break;
        case 252: //hidden feature hack for devs
            ret = SLOW5_COMPRESS_EX_ZD;
            break;
        default: //todo: Proper error?
            ret = 255;
            SLOW5_WARNING("Unknown signal compression method %d",method);
            break;
    }
    return ret;
}


/* --- Init / free (__)slow5_press structures --- */

/*
 * init slow5_press struct
 * returns NULL on error and sets errno
 * errors
 * SLOW5_ERR_MEM
 * SLOW5_ERR_ARG    method is bad
 * SLOW5_ERR_PRESS  (de)compression failure
 */
struct slow5_press *slow5_press_init(slow5_press_method_t method) {

    struct __slow5_press *record_comp = __slow5_press_init(method.record_method);
    if (!record_comp) {
        return NULL;
    }

    struct __slow5_press *signal_comp = __slow5_press_init(method.signal_method);
    if (!signal_comp) {
        __slow5_press_free(record_comp);
        return NULL;
    }

    struct slow5_press *comp = (struct slow5_press *) calloc(1, sizeof *comp);
    if (!comp) {
        SLOW5_MALLOC_ERROR();
        __slow5_press_free(record_comp);
        __slow5_press_free(signal_comp);
        slow5_errno = SLOW5_ERR_MEM;
        return NULL;
    }
    comp->record_press = record_comp;
    comp->signal_press = signal_comp;

    return comp;
}

void slow5_press_free(struct slow5_press *comp) {

    if (comp) {
        __slow5_press_free(comp->record_press);
        __slow5_press_free(comp->signal_press);
        free(comp);
    }
}

/*
 * init __slow5_press struct
 * returns NULL on error and sets errno
 * errors
 * SLOW5_ERR_MEM
 * SLOW5_ERR_ARG    method is bad
 * SLOW5_ERR_PRESS  (de)compression failure
 */
struct __slow5_press *__slow5_press_init(enum slow5_press_method method) {

    struct __slow5_press *comp = NULL;

    comp = (struct __slow5_press *) calloc(1, sizeof *comp);
    if (!comp) {
        SLOW5_MALLOC_ERROR();
        slow5_errno = SLOW5_ERR_MEM;
        return NULL;
    }
    comp->method = method;

    switch (method) {
        case SLOW5_COMPRESS_NONE: break;
        case SLOW5_COMPRESS_ZLIB:
            {
            struct slow5_zlib_stream *zlib;

            zlib = (struct slow5_zlib_stream *) malloc(sizeof *zlib);
            if (!zlib) {
                SLOW5_MALLOC_ERROR();
                free(comp);
                slow5_errno = SLOW5_ERR_MEM;
                return NULL;
            }

            if (zlib_init_deflate(&(zlib->strm_deflate)) != Z_OK) {
                SLOW5_ERROR("zlib deflate init failed: %s.", zlib->strm_deflate.msg);
                free(zlib);
                free(comp);
                slow5_errno = SLOW5_ERR_PRESS;
                return NULL;
            }
            if (zlib_init_inflate(&(zlib->strm_inflate)) != Z_OK) {
                SLOW5_ERROR("zlib inflate init failed: %s.", zlib->strm_inflate.msg);
                if (deflateEnd(&(zlib->strm_deflate)) != Z_OK) {
                    SLOW5_ERROR("zlib deflate end failed: %s.", zlib->strm_deflate.msg);
                }
                free(zlib);
                free(comp);
                slow5_errno = SLOW5_ERR_PRESS;
                return NULL;
            }

            zlib->flush = Z_NO_FLUSH;
            comp->stream = (union slow5_press_stream *) malloc(sizeof *comp->stream);
            if (!comp->stream) {
                SLOW5_MALLOC_ERROR();
                if (deflateEnd(&(zlib->strm_deflate)) != Z_OK) {
                    SLOW5_ERROR("zlib deflate end failed: %s.", zlib->strm_deflate.msg);
                }
                if (inflateEnd(&(zlib->strm_inflate)) != Z_OK) {
                    SLOW5_ERROR("zlib inflate end failed: %s.", zlib->strm_inflate.msg);
                }
                free(zlib);
                free(comp);
                slow5_errno = SLOW5_ERR_PRESS;
                return NULL;
            }

            comp->stream->zlib = zlib;
            } break;

        case SLOW5_COMPRESS_SVB_ZD: break;
        case SLOW5_COMPRESS_ZSTD:
#ifdef SLOW5_USE_ZSTD
            break;
#else
            SLOW5_ERROR("%s","slow5lib has not been compiled with zstd support to read/write zstd compressed BLOW5 files.");
            free(comp);
            slow5_errno = SLOW5_ERR_ARG;
            return NULL;
#endif /* SLOW5_USE_ZSTD */
        case SLOW5_COMPRESS_EX_ZD: break;
        default:
            SLOW5_ERROR("Invalid or unsupported (de)compression method '%d'.", method);
            free(comp);
            slow5_errno = SLOW5_ERR_ARG;
            return NULL;
    }

    return comp;
}

void __slow5_press_free(struct __slow5_press *comp) {

    if (comp) {

        switch (comp->method) {

            case SLOW5_COMPRESS_NONE: break;
            case SLOW5_COMPRESS_ZLIB:
                (void) deflateEnd(&(comp->stream->zlib->strm_deflate));
                (void) inflateEnd(&(comp->stream->zlib->strm_inflate));
                free(comp->stream->zlib);
                free(comp->stream);
                break;
            case SLOW5_COMPRESS_SVB_ZD: break;
#ifdef SLOW5_USE_ZSTD
            case SLOW5_COMPRESS_ZSTD: break;
#endif /* SLOW5_USE_ZSTD */
            case SLOW5_COMPRESS_EX_ZD: break;
            default:
                SLOW5_ERROR("Invalid or unsupported (de)compression method '%d'.", comp->method);
                slow5_errno = SLOW5_ERR_ARG;
                break;
        }

        free(comp);
    }
}

void *slow5_ptr_compress_solo(enum slow5_press_method method, const void *ptr, size_t count, size_t *n) {
    void *out = NULL;
    size_t n_tmp = 0;

    if (!ptr) {
        SLOW5_ERROR("Argument '%s' cannot be NULL.", SLOW5_TO_STR(ptr))
        slow5_errno = SLOW5_ERR_ARG;
    } else {
        switch (method) {

            case SLOW5_COMPRESS_NONE:
                out = (void *) malloc(count);
                SLOW5_MALLOC_CHK(out);
                if (!out) {
                    // Malloc failed
                    return out;
                }
                memcpy(out, ptr, count);
                n_tmp = count;
                break;

            case SLOW5_COMPRESS_ZLIB:
                out = ptr_compress_zlib_solo(ptr, count, &n_tmp);
                break;

            case SLOW5_COMPRESS_SVB_ZD:
                out = ptr_compress_svb_zd(ptr, count, &n_tmp);
                break;

#ifdef SLOW5_USE_ZSTD
            case SLOW5_COMPRESS_ZSTD:
                out = ptr_compress_zstd(ptr, count, &n_tmp);
                break;
#endif /* SLOW5_USE_ZSTD */
            case SLOW5_COMPRESS_EX_ZD:
                out = ptr_compress_ex_zd(ptr, count, &n_tmp);
                break;
            default:
                SLOW5_ERROR("Invalid or unsupported (de)compression method '%d'.", method);
                slow5_errno = SLOW5_ERR_ARG;
                break;
        }
    }

    if (n) {
        *n = n_tmp;
    }

    return out;
}

/* --- Compress / decompress to some memory --- */

void *slow5_ptr_compress(struct __slow5_press *comp, const void *ptr, size_t count, size_t *n) {
    void *out = NULL;
    size_t n_tmp = 0;

    if (comp && ptr) {

        switch (comp->method) {

            case SLOW5_COMPRESS_NONE:
                out = (void *) malloc(count);
                SLOW5_MALLOC_CHK(out);
                if (!out) {
                    // Malloc failed
                    return out;
                }
                memcpy(out, ptr, count);
                n_tmp = count;
                break;

            case SLOW5_COMPRESS_ZLIB:
                if (comp->stream && comp->stream->zlib) {
                    out = ptr_compress_zlib(comp->stream->zlib, ptr, count, &n_tmp);
                }
                break;

            case SLOW5_COMPRESS_SVB_ZD:
                out = ptr_compress_svb_zd(ptr, count, &n_tmp);
                break;

#ifdef SLOW5_USE_ZSTD
            case SLOW5_COMPRESS_ZSTD:
                out = ptr_compress_zstd(ptr, count, &n_tmp);
                break;
#endif /* SLOW5_USE_ZSTD */

            case SLOW5_COMPRESS_EX_ZD:
                out = ptr_compress_ex_zd(ptr, count, &n_tmp);
                break;

            default:
                SLOW5_ERROR("Invalid or unsupported (de)compression method '%d'.", comp->method);
                slow5_errno = SLOW5_ERR_ARG;
                break;
        }
    }

    if (n) {
        *n = n_tmp;
    }

    return out;
}

/*
 * ptr cannot be NULL
 */
void *slow5_ptr_depress_solo(enum slow5_press_method method, const void *ptr, size_t count, size_t *n) {
    void *out = NULL;
    size_t n_tmp = 0;

    if (!ptr) {
        SLOW5_ERROR("Argument '%s' cannot be NULL.", SLOW5_TO_STR(ptr))
        slow5_errno = SLOW5_ERR_ARG;
    } else {

        switch (method) {

            case SLOW5_COMPRESS_NONE:
                out = (void *) malloc(count);
                SLOW5_MALLOC_CHK(out);
                if (!out) {
                    /* Malloc failed */
                    return out;
                }
                memcpy(out, ptr, count);
                n_tmp = count;
                break;

            case SLOW5_COMPRESS_ZLIB:
                out = ptr_depress_zlib_solo(ptr, count, &n_tmp);
                break;

            case SLOW5_COMPRESS_SVB_ZD:
                out = ptr_depress_svb_zd(ptr, count, &n_tmp);
                break;

#ifdef SLOW5_USE_ZSTD
            case SLOW5_COMPRESS_ZSTD:
                out = ptr_depress_zstd(ptr, count, &n_tmp);
                break;
#endif /* SLOW5_USE_ZSTD */

            case SLOW5_COMPRESS_EX_ZD:
                out = ptr_depress_ex_zd(ptr, count, &n_tmp);
                break;

            default:
                SLOW5_ERROR("Invalid or unsupported (de)compression method '%d'.", method);
                slow5_errno = SLOW5_ERR_ARG;
                break;
        }
    }

    if (n) {
        *n = n_tmp;
    }

    return out;
}

/*
 * decompress count bytes of a ptr to compressed memory
 * returns pointer to decompressed memory of size *n bytes to be later freed
 * returns NULL on error and *n set to 0
 * DONE
 */
void *slow5_ptr_depress(struct __slow5_press *comp, const void *ptr, size_t count, size_t *n) {
    void *out = NULL;

    if (!comp || !ptr) {
        if (!comp) {
            SLOW5_ERROR("Argument '%s' cannot be NULL.", SLOW5_TO_STR(comp));
        }
        if (!ptr) {
            SLOW5_ERROR("Argument '%s' cannot be NULL.", SLOW5_TO_STR(ptr));
        }
        if (n) {
            *n = 0;
        }
        slow5_errno = SLOW5_ERR_ARG;
        return NULL;
    } else {
        size_t n_tmp = 0;

        switch (comp->method) {

            case SLOW5_COMPRESS_NONE:
                out = (void *) malloc(count);
                SLOW5_MALLOC_CHK(out);
                if (!out) { /* malloc failed */
                    if (n) {
                        *n = 0;
                    }
                    return out;
                }
                memcpy(out, ptr, count);
                if (n) {
                    *n = count;
                }
                break;

            case SLOW5_COMPRESS_ZLIB:
                if (!comp->stream) {
                    SLOW5_ERROR("%s", "Decompression stream cannot be NULL.")
                } else {
                    out = ptr_depress_zlib(comp->stream->zlib, ptr, count, &n_tmp);
                    if (!out) {
                        SLOW5_ERROR("%s", "zlib decompression failed.")
                    }
                }
                break;

            case SLOW5_COMPRESS_SVB_ZD:
                out = ptr_depress_svb_zd(ptr, count, &n_tmp);
                break;

#ifdef SLOW5_USE_ZSTD
            case SLOW5_COMPRESS_ZSTD:
                out = ptr_depress_zstd(ptr, count, &n_tmp);
                break;
#endif /* SLOW5_USE_ZSTD */

            case SLOW5_COMPRESS_EX_ZD:
                out = ptr_depress_ex_zd(ptr, count, &n_tmp);
                break;

            default:
                SLOW5_ERROR("Invalid or unsupported (de)compression method '%d'.", comp->method);
                slow5_errno = SLOW5_ERR_ARG;
                break;
        }

        if (n) {
            *n = n_tmp;
        }
    }

    return out;
}

/* --- Compress / decompress a ptr to some file --- */

/* return -1 on failure */
ssize_t slow5_fwrite_compress(struct __slow5_press *comp, const void *ptr, size_t size, size_t nmemb, FILE *fp) {
    ssize_t bytes = -1;
    size_t bytes_tmp = 0;
    void *out = NULL;

    if (comp) {
        switch (comp->method) {

            case SLOW5_COMPRESS_NONE:
                bytes = fwrite(ptr, size, nmemb, fp);
                if (bytes != size * nmemb || ferror(fp)) {
                    if (bytes != size * nmemb) {
                        SLOW5_ERROR("Expected to write '%zu' bytes, instead wrote '%zu' bytes.",
                                size * nmemb, bytes);
                    } else {
                        SLOW5_ERROR("%s", "File error after trying to write.");
                    }
                    slow5_errno = SLOW5_ERR_IO;
                    return -1;
                }
                break;

            case SLOW5_COMPRESS_ZLIB:
                if (comp->stream && comp->stream->zlib) {
                    bytes = fwrite_compress_zlib(comp->stream->zlib, ptr, size, nmemb, fp);
                }
                break;

            case SLOW5_COMPRESS_SVB_ZD:
                out = ptr_compress_svb_zd(ptr, size * nmemb, &bytes_tmp);
                if (!out) {
                    return -1;
                }
                break;

#ifdef SLOW5_USE_ZSTD
            case SLOW5_COMPRESS_ZSTD:
                out = ptr_compress_zstd(ptr, size * nmemb, &bytes_tmp);
                if (!out) {
                    return -1;
                }
                break;
#endif /* SLOW5_USE_ZSTD */

            default:
                SLOW5_ERROR("Invalid or unsupported (de)compression method '%d'.", comp->method);
                slow5_errno = SLOW5_ERR_ARG;
                return -1;
        }
    }

    if (out) {
        size_t bytes_written = fwrite(out, 1, bytes_tmp, fp);
        free(out);
        if (bytes_written != bytes_tmp || ferror(fp)) {
            if (bytes_written != bytes_tmp) {
                SLOW5_ERROR("Expected to write '%zu' compressed bytes, instead wrote '%zu' bytes.",
                        bytes_tmp, bytes_written);
            } else {
                SLOW5_ERROR("%s", "File error after trying to write.");
            }
            slow5_errno = SLOW5_ERR_IO;
            return -1;
        }
    }

    return bytes = (ssize_t) bytes_tmp;
}

/*
 * decompress count bytes from some file
 * returns pointer to decompressed memory of size *n bytes to be later freed
 * returns NULL on error and *n set to 0
 * DONE
 */
void *slow5_fread_depress(struct __slow5_press *comp, size_t count, FILE *fp, size_t *n) {
    void *raw = (void *) malloc(count);
    SLOW5_MALLOC_CHK(raw);
    if (!raw) {
        return NULL;
    }

    if (fread(raw, count, 1, fp) != 1) {
        SLOW5_ERROR("Failed to read '%zu' bytes from file.", count);
        free(raw);
        return NULL;
    }

    void *out = slow5_ptr_depress(comp, raw, count, n);
    if (!out) {
        SLOW5_ERROR("%s", "Decompression failed.")
    }
    free(raw);

    return out;
}
/*
static void *fread_depress_solo(slow5_press_method_t method, size_t count, FILE *fp, size_t *n) {
    void *raw = (void *) malloc(count);
    SLOW5_MALLOC_CHK(raw);

    if (fread(raw, count, 1, fp) != 1) {
        free(raw);
        return NULL;
    }

    void *out = slow5_ptr_depress_solo(method, raw, count, n);
    free(raw);

    return out;
}
*/
void *slow5_pread_depress(struct __slow5_press *comp, int fd, size_t count, off_t offset, size_t *n) {
    void *raw = (void *) malloc(count);
    SLOW5_MALLOC_CHK(raw);

    if (pread(fd, raw, count, offset) == -1) {
        free(raw);
        return NULL;
    }

    void *out = slow5_ptr_depress(comp, raw, count, n);
    free(raw);

    return out;
}

/*
 * pread size bytes from offset then decompress the data according to the method
 */
void *slow5_pread_depress_solo(enum slow5_press_method method, int fd, size_t count, off_t offset, size_t *n) {
    void *raw = (void *) malloc(count);
    SLOW5_MALLOC_CHK(raw);
    if (!raw) {
        return NULL;
    }

    ssize_t ret;
    if ((ret = pread(fd, raw, count, offset)) != count) {
        if (ret == -1) {
            SLOW5_ERROR("pread failed to read '%zu' bytes: %s", count, strerror(errno));
        } else if (ret == 0) {
            SLOW5_ERROR("End of file reached. pread failed to read '%zu' bytes.", count);
        } else {
            SLOW5_ERROR("pread read less bytes '%zd' than expected '%zu'.", ret, count);
        }
        free(raw);
        return NULL;
    }

    void *out = slow5_ptr_depress_solo(method, raw, count, n);
    free(raw);

    return out;
}


/* --- Compress with printf to some file --- */

int slow5_fprintf_compress(struct __slow5_press *comp, FILE *fp, const char *format, ...) {
    int ret = -1;

    va_list ap;
    va_start(ap, format);

    ret = vfprintf_compress(comp, fp, format, ap);

    va_end(ap);

    return ret;
}

int slow5_printf_compress(struct __slow5_press *comp, const char *format, ...) {
    int ret = -1;

    va_list ap;
    va_start(ap, format);

    ret = vfprintf_compress(comp, stdout, format, ap);

    va_end(ap);

    return ret;
}

static int vfprintf_compress(struct __slow5_press *comp, FILE *fp, const char *format, va_list ap) {
    int ret = -1;

    if (comp) {

        if (comp->method == SLOW5_COMPRESS_NONE) {
            ret = vfprintf(fp, format, ap);
        } else {
            char *buf;
            if (slow5_vasprintf(&buf, format, ap) != -1) {
                ret = slow5_fwrite_str_compress(comp, buf, fp);
                free(buf);
            }
        }

    }

    return ret;
}


/* --- Write compression footer on immediate next compression call --- */

void slow5_compress_footer_next(struct __slow5_press *comp) {

    if (comp && comp->stream) {
        switch (comp->method) {
            case SLOW5_COMPRESS_NONE: break;
            case SLOW5_COMPRESS_ZLIB: {
                struct slow5_zlib_stream *zlib = comp->stream->zlib;

                if (zlib) {
                    zlib->flush = Z_FINISH;
                }
            } break;
            case SLOW5_COMPRESS_SVB_ZD: break;
#ifdef SLOW5_USE_ZSTD
            case SLOW5_COMPRESS_ZSTD: break;
#endif /* SLOW5_USE_ZSTD */

            default:
                SLOW5_ERROR("Invalid or unsupported (de)compression method '%d'.", comp->method);
                slow5_errno = SLOW5_ERR_ARG;
                break;
        }
    }
}




/********
 * ZLIB *
 ********/

static int zlib_init_deflate(z_stream *strm) {
    strm->zalloc = Z_NULL;
    strm->zfree = Z_NULL;
    strm->opaque = Z_NULL;

    return deflateInit2(strm,
            Z_DEFAULT_COMPRESSION,
            Z_DEFLATED,
            MAX_WBITS,
            SLOW5_ZLIB_MEM_DEFAULT,
            Z_DEFAULT_STRATEGY);
}

static int zlib_init_inflate(z_stream *strm) {
    strm->zalloc = Z_NULL;
    strm->zfree = Z_NULL;
    strm->opaque = Z_NULL;

    return inflateInit2(strm, MAX_WBITS);
}

static void *ptr_compress_zlib(struct slow5_zlib_stream *zlib, const void *ptr, size_t count, size_t *n) {
    uint8_t *out = NULL;

    size_t n_cur = 0;
    z_stream *strm = &(zlib->strm_deflate);

    strm->avail_in = count;
    strm->next_in = (Bytef *) ptr;

    uLong chunk_sz = SLOW5_ZLIB_COMPRESS_CHUNK;

    do {
        out = (uint8_t *) realloc(out, n_cur + chunk_sz);
        SLOW5_MALLOC_CHK(out);

        strm->avail_out = chunk_sz;
        strm->next_out = out + n_cur;

        if (deflate(strm, zlib->flush) == Z_STREAM_ERROR) {
            free(out);
            out = NULL;
            n_cur = 0;
            break;
        }

        n_cur += chunk_sz - strm->avail_out;

    } while (strm->avail_out == 0);

    *n = n_cur;

    if (zlib->flush == Z_FINISH) {
        zlib->flush = Z_NO_FLUSH;
        deflateReset(strm);
    }

    return out;
}

static void *ptr_compress_zlib_solo(const void *ptr, size_t count, size_t *n) {
    uint8_t *out = NULL;

    size_t n_cur = 0;

    z_stream strm_local;
    zlib_init_deflate(&strm_local);
    z_stream *strm = &strm_local;

    strm->avail_in = count;
    strm->next_in = (Bytef *) ptr;

    uLong chunk_sz = SLOW5_ZLIB_COMPRESS_CHUNK;

    do {
        out = (uint8_t *) realloc(out, n_cur + chunk_sz);
        SLOW5_MALLOC_CHK(out);

        strm->avail_out = chunk_sz;
        strm->next_out = out + n_cur;

        if (deflate(strm, Z_FINISH) == Z_STREAM_ERROR) {
            free(out);
            out = NULL;
            n_cur = 0;
            break;
        }

        n_cur += chunk_sz - strm->avail_out;

    } while (strm->avail_out == 0);

    *n = n_cur;

    (void) inflateEnd(strm);

    return out;
}

/*
 * zlib decompress count bytes of a ptr to compressed memory
 * returns pointer to decompressed memory of size *n bytes to be later freed
 * returns NULL on error and *n set to 0
 * DONE
 */
static void *ptr_depress_zlib(struct slow5_zlib_stream *zlib, const void *ptr, size_t count, size_t *n) {
    uint8_t *out = NULL;

    size_t n_cur = 0;
    if (!zlib) {
        SLOW5_ERROR("%s", "zlib stream cannot be NULL.")
        return NULL;
    }
    z_stream *strm = &(zlib->strm_inflate);
    if (!strm) {
        SLOW5_ERROR("%s", "zlib inflate stream cannot be NULL.")
        return NULL;
    }

    strm->avail_in = count;
    strm->next_in = (Bytef *) ptr;

    do {
        uint8_t *out_new = (uint8_t *) realloc(out, n_cur + SLOW5_ZLIB_DEPRESS_CHUNK);
        SLOW5_MALLOC_CHK(out_new);
        if (!out_new) {
            free(out);
            out = NULL;
            n_cur = 0;
            break;
        }
        out = out_new;

        strm->avail_out = SLOW5_ZLIB_DEPRESS_CHUNK;
        strm->next_out = out + n_cur;

        int ret = inflate(strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR) {
            SLOW5_ERROR("zlib inflate failed with error code %d.", ret)
            free(out);
            out = NULL;
            n_cur = 0;
            break;
        }

        n_cur += SLOW5_ZLIB_DEPRESS_CHUNK- strm->avail_out;

    } while (strm->avail_out == 0);

    *n = n_cur;
    if (out && inflateReset(strm) == Z_STREAM_ERROR) {
        SLOW5_WARNING("%s", "Stream state is inconsistent.")
    };

    return out;
}

static void *ptr_depress_zlib_solo(const void *ptr, size_t count, size_t *n) {
    uint8_t *out = NULL;

    size_t n_cur = 0;

    z_stream strm_local;
    zlib_init_inflate(&strm_local);
    z_stream *strm = &strm_local;

    strm->avail_in = count;
    strm->next_in = (Bytef *) ptr;

    do {
        out = (uint8_t *) realloc(out, n_cur + SLOW5_ZLIB_DEPRESS_CHUNK);
        SLOW5_MALLOC_CHK(out);

        strm->avail_out = SLOW5_ZLIB_DEPRESS_CHUNK;
        strm->next_out = out + n_cur;

        int ret = inflate(strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_NEED_DICT || ret == Z_MEM_ERROR) {
            SLOW5_ERROR("%s","inflate failed");
            free(out);
            out = NULL;
            n_cur = 0;
            break;
        }

        n_cur += SLOW5_ZLIB_DEPRESS_CHUNK- strm->avail_out;

    } while (strm->avail_out == 0);

    *n = n_cur;

    (void) inflateEnd(strm);

    return out;
}

static ssize_t fwrite_compress_zlib(struct slow5_zlib_stream *zlib, const void *ptr, size_t size, size_t nmemb, FILE *fp) {

    ssize_t bytes = 0;
    z_stream *strm = &(zlib->strm_deflate);

    strm->avail_in = size * nmemb;
    strm->next_in = (Bytef *) ptr;

    uLong chunk_sz = SLOW5_ZLIB_COMPRESS_CHUNK;
    uint8_t *buf = (uint8_t *) malloc(sizeof *buf * chunk_sz);
    SLOW5_MALLOC_CHK(buf);
    if (!buf) {
        return -1;
    }

    do {
        strm->avail_out = chunk_sz;
        strm->next_out = buf;

        if (deflate(strm, zlib->flush) == Z_STREAM_ERROR) {
            bytes = -1;
            break;
        }

        size_t have = (sizeof *buf * chunk_sz) - strm->avail_out;
        if (fwrite(buf, sizeof *buf, have, fp) != have || ferror(fp)) {
            bytes = -1;
            break;
        }

        bytes += have;

    } while (strm->avail_out == 0);

    free(buf);

    if (zlib->flush == Z_FINISH) {
        zlib->flush = Z_NO_FLUSH;
    }

    return bytes;
}



/***************
 * STREAMVBYTE *
 ***************/

/* return NULL on malloc error, n cannot be NULL */
static uint8_t *ptr_compress_svb(const uint32_t *ptr, size_t count, size_t *n) {
    uint32_t length = count / sizeof *ptr;

    size_t max_n = __slow5_streamvbyte_max_compressedbytes(length);
    uint8_t *out = (uint8_t *) malloc(max_n + sizeof length);
    if (!out) {
        SLOW5_MALLOC_ERROR();
        slow5_errno = SLOW5_ERR_MEM;
        return NULL;
    }

    *n = __slow5_streamvbyte_encode(ptr, length, out + sizeof length);
    memcpy(out, &length, sizeof length); /* copy original length of ptr (needed for depress) */
    *n = *n + sizeof length;
    SLOW5_LOG_DEBUG("max svb bytes=%zu\nsvb bytes=%zu\n",
            max_n, *n); /* TESTING */
    return out;
}

/* return NULL on malloc error, n cannot be NULL */
static uint8_t *ptr_compress_svb_zd(const int16_t *ptr, size_t count, size_t *n) {
    uint32_t length = count / sizeof *ptr;
    int32_t *in = (int32_t *) malloc(length * sizeof *in);
    if (!in) {
        SLOW5_MALLOC_ERROR();
        slow5_errno = SLOW5_ERR_MEM;
        return NULL;
    }
    for (int64_t i = 0; i < length; ++ i) {
        in[i] = ptr[i];
    }

    uint32_t *diff = (uint32_t *) malloc(length * sizeof *diff);
    if (!diff) {
        SLOW5_MALLOC_ERROR();
        free(in);
        slow5_errno = SLOW5_ERR_MEM;
        return NULL;
    }
    __slow5_zigzag_delta_encode(in, diff, length, 0);


    SLOW5_LOG_DEBUG("orig bytes=%zu\n", count); /* TESTING */
    uint8_t *out = ptr_compress_svb(diff, length * sizeof *diff, n);

    free(in);
    free(diff);
    return out;
}

/* return NULL on malloc error, n cannot be NULL */
static uint32_t *ptr_depress_svb(const uint8_t *ptr, size_t count, size_t *n) {
    uint32_t length;
    memcpy(&length, ptr, sizeof length); /* get original array length */

    uint32_t *out = (uint32_t *) malloc(length * sizeof *out);
    if (!out) {
        SLOW5_MALLOC_ERROR();
        slow5_errno = SLOW5_ERR_MEM;
        return NULL;
    }

    size_t bytes_read;
    if ((bytes_read = __slow5_streamvbyte_decode(ptr + sizeof length, out, length)) != count - sizeof length) {
        SLOW5_ERROR("Expected streamvbyte_decode to read '%zu' bytes, instead read '%zu' bytes.",
                count - sizeof length, bytes_read);
        slow5_errno = SLOW5_ERR_PRESS;
        free(out);
        return NULL;
    }

    *n = length * sizeof *out;
    return out;
}

/* return NULL on malloc error, n cannot be NULL */
static int16_t *ptr_depress_svb_zd(const uint8_t *ptr, size_t count, size_t *n) {

    uint32_t *diff = ptr_depress_svb(ptr, count, n);
    if (!diff) {
        return NULL;
    }
    uint32_t length = *n / sizeof *diff;

    int16_t *orig = (int16_t *) malloc(length * sizeof *orig);
    if (!orig) {
        SLOW5_MALLOC_ERROR();
        free(diff);
        slow5_errno = SLOW5_ERR_MEM;
        return NULL;
    }
    __slow5_zigzag_delta_decode(diff, orig, length, 0);

    // int16_t *orig = (int16_t *) malloc(length * sizeof *orig);
    // for (int64_t i = 0; i < length; ++ i) {
    //     orig[i] = out[i];
    // }

    *n = length * sizeof *orig;
    free(diff);
    //free(out);
    return orig;
}



#ifdef SLOW5_USE_ZSTD
/********
 * ZSTD *
 ********/

/* return NULL on error */
static void *ptr_compress_zstd(const void *ptr, size_t count, size_t *n) {
    size_t max_bytes = ZSTD_compressBound(count);

    void *out = malloc(max_bytes);
    if (!out) {
        SLOW5_MALLOC_ERROR();
        slow5_errno = SLOW5_ERR_MEM;
        return NULL;
    }

    *n = ZSTD_compress(out, max_bytes, ptr, count, SLOW5_ZSTD_COMPRESS_LEVEL);
    if (ZSTD_isError(*n)) {
        SLOW5_ERROR("zstd compress failed with error code %zu.", *n);
        free(out);
        slow5_errno = SLOW5_ERR_PRESS;
        return NULL;
    }

    return out;
}

/* return NULL on error */
static void *ptr_depress_zstd(const void *ptr, size_t count, size_t *n) {
    unsigned long long depress_bytes = ZSTD_getFrameContentSize(ptr, count);
    if (depress_bytes == ZSTD_CONTENTSIZE_UNKNOWN ||
            depress_bytes == ZSTD_CONTENTSIZE_ERROR) {
        SLOW5_ERROR("zstd get decompressed size failed with error code %llu\n", depress_bytes);
        slow5_errno = SLOW5_ERR_PRESS;
        return NULL;
    }

    void *out = malloc(depress_bytes);
    if (!out) {
        SLOW5_MALLOC_ERROR();
        slow5_errno = SLOW5_ERR_MEM;
        return NULL;
    }

    *n = ZSTD_decompress(out, depress_bytes, ptr, count);
    if (ZSTD_isError(*n)) {
        SLOW5_ERROR("zstd decompress failed with error code %zu.", *n);
        free(out);
        slow5_errno = SLOW5_ERR_PRESS;
        return NULL;
    }

    return out;
}
#endif /* SLOW5_USE_ZSTD */

/* EX_ZD */

/*
 * variable byte 1 except 2 before bitpack
 * [num exceptions]
 * [num bytes of next block][exception indices | diff - 1 | bitpack]
 * [num bytes of next block][exceptions - 256 | bitpack]
 * data
 * maximum 256 exceptions
 */

#define SLOW5_EX_MAX_EXCEPTIONS (UINT32_MAX)

static uint32_t *delta_increasing_u32(const uint32_t *in, uint64_t nin)
{
	uint32_t prev;
	uint32_t *out;
	uint64_t i;

	if (!nin)
		return NULL;

	out = malloc(nin * sizeof *out);

	out[0] = in[0];
	prev = in[0];

	for (i = 1; i < nin; i++) {
		out[i] = in[i] - prev - 1;
		prev = in[i];
	}

	return out;
}

static uint32_t get_max_u32(const uint32_t *in, uint64_t nin)
{
	uint32_t max;
	uint64_t i;

	max = 0;

	for (i = 0; i < nin; i++) {
		if (in[i] > max)
			max = in[i];
	}

	return max;
}


#define SLOW5_DIV_ROUND_UP(n, d)	((n - 1) / d + 1)
#define SLOW5_BITS_PER_BYTE		(8)
#define SLOW5_BITS_TO_BYTES(n)	SLOW5_DIV_ROUND_UP(n, SLOW5_BITS_PER_BYTE)
#define SLOW5_BYTES_TO_BITS(n)	((n) * SLOW5_BITS_PER_BYTE)

/* uintx */

static uint64_t uintx_bound(uint8_t in_bits, uint8_t out_bits, uint64_t nin)
{
	uint64_t nin_elems;

	nin_elems = SLOW5_BYTES_TO_BITS(nin) / in_bits;

	if (!out_bits)
		return 0;

	return SLOW5_BITS_TO_BYTES(nin_elems * out_bits);
}


static void uintx_update(uint8_t in_bits, uint8_t out_bits, uint64_t *in_i,
		  uint64_t *out_i, uint8_t *in_bits_free,
		  uint8_t *out_bits_free, uint8_t *bits_left)
{
	int8_t gap;

	gap = in_bits - out_bits;

	if (gap > 0) {
		if (gap > *in_bits_free) {
			(*in_i)++;
			gap -= *in_bits_free;
		}
		*bits_left = out_bits;
		*in_bits_free = SLOW5_BITS_PER_BYTE - gap % SLOW5_BITS_PER_BYTE;
		*in_i += gap / SLOW5_BITS_PER_BYTE;
	} else {
		gap *= -1;
		if (gap > *out_bits_free) {
			(*out_i)++;
			gap -= *out_bits_free;
		}
		*bits_left = in_bits;
		*out_bits_free = SLOW5_BITS_PER_BYTE - gap % SLOW5_BITS_PER_BYTE;
		*out_i += gap / SLOW5_BITS_PER_BYTE;
	}
}

/* if in_bits > out_bits: in must be in big endian format */
static int uintx_press_core(uint8_t in_bits, uint8_t out_bits, const uint8_t *in,
		     uint64_t nin, uint8_t *out, uint64_t *nout)
{
	/*
	 * in_bits = 16
	 * out_bits = 11
	 * gap = 5
	 * in_free_bits = 8
	 * in = [00000{010, 10001011}, ...]
	 * out = [{01010001, 011}..., ...]
	 *
	 * press operations on P11:
	 * out[0] = in[0] << 5 | in[1] >> 3;
	 * out[1] = in[1] << 5 | in[2] >> 3;
	 * ...
	 *
	 * in_bits = 14
	 * out_bits = 9
	 * gap = 5
	 * in_free_bits = 8,2
	 * in = [00000{010, 100010}00, 000 ...]
	 * out = [{01010001, 0}..., ...]
	 *
	 * reverse:
	 * in_bits = 11
	 * out_bits = 16
	 * gap = -5
	 * in = [{01010001, 011}..., ...]
	 * out = [00000{010, 10001011}, ...]
	 *
	 * press operations on compressed P11:
	 * out[0] = in[0] >> 5
	 * out[1] = in[0] << 3 | in[1] >> 5;
	 * out[2] = in[1] << 3 | in[2] >> 5;
	 * ...
	 */

	int dirty;
	int8_t gap;
	uint64_t i;
	uint64_t in_i;
	uint64_t out_i;
	uint8_t bits_left;
	uint8_t cur_out;
	uint8_t in_bits_free;
	uint8_t mask;
	uint8_t out_bits_free;

	/* when decompressing some of out may be skipped */
	if (in_bits < out_bits)
		(void) memset(out, 0, *nout);

	bits_left = 0;
	cur_out = 0;
	dirty = 0;
	i = 0;
	in_bits_free = SLOW5_BITS_PER_BYTE;
	in_i = 0;
	out_bits_free = SLOW5_BITS_PER_BYTE;
	out_i = 0;

	uintx_update(in_bits, out_bits, &in_i, &out_i, &in_bits_free,
		     &out_bits_free, &bits_left);

	while (i < nin) {
		mask = 0xFF >> (SLOW5_BITS_PER_BYTE - in_bits_free);
		gap = in_bits_free - out_bits_free;
		if (gap > 0) {
			cur_out |= (in[in_i] & mask) >> gap;
			in_bits_free -= out_bits_free;
			bits_left -= out_bits_free;
			out_bits_free = 0;
		} else {
			cur_out |= (in[in_i] & mask) << (-1 * gap);
			out_bits_free -= in_bits_free;
			bits_left -= in_bits_free;
			in_bits_free = 0;
		}
		dirty = 1;

		if (!in_bits_free) {
			in_bits_free = SLOW5_BITS_PER_BYTE;
			in_i++;
		}

		if (!out_bits_free) {
			if (out_i == *nout)
				return -1;
			out[out_i++] = cur_out;
			out_bits_free = SLOW5_BITS_PER_BYTE;
			cur_out = 0;
			dirty = 0;
		}

		if (!bits_left) {
			uintx_update(in_bits, out_bits, &in_i, &out_i,
				     &in_bits_free, &out_bits_free,
				     &bits_left);
			i++;
		}
	}

	/* if there is still data to flush */
	if (dirty) {
		if (out_i == *nout)
			return -1;
		out[out_i++] = cur_out;
	}

	*nout = out_i;

	return 0;
}

static inline uint16_t uint16_bswap(uint16_t x) {
	return (x<<8) | (x>>8);
}

static inline uint32_t uint32_bswap(uint32_t x) {
	return (x>>24) | (x>>8 & 0xff00) | (x<<8 & 0xff0000) | (x<<24);
}

static inline uint64_t uint64_bswap(uint64_t x){
	return ((uint32_bswap(x)+0ULL) << 32) | uint32_bswap(x>>32);
}

#define slow5_letobe16(x) uint16_bswap(x)
#define slow5_be16tole(x) uint16_bswap(x)
#define slow5_letobe32(x) uint32_bswap(x)
#define slow5_be32tole(x) uint32_bswap(x)
#define slow5_letobe64(x) uint64_bswap(x)
#define slow5_be64tole(x) uint64_bswap(x)

/* copy h from host endian into be as big endian */
static void uintx_htobe(uint8_t in_bits, const uint8_t *h, uint8_t *be, uint64_t n)
{
	uint64_t i;

	if (in_bits <= SLOW5_BYTES_TO_BITS(sizeof (uint8_t))) {
		return;
	} else if (in_bits <= SLOW5_BYTES_TO_BITS(sizeof (uint16_t))) {
		for (i = 0; i < n / sizeof (uint16_t); i++) {
			((uint16_t *) be)[i] = slow5_letobe16(((uint16_t *) h)[i]);
		}
	} else if (in_bits <= SLOW5_BYTES_TO_BITS(sizeof (uint32_t))) {
		for (i = 0; i < n / sizeof (uint32_t); i++) {
			((uint32_t *) be)[i] = slow5_letobe32(((uint32_t *) h)[i]);
		}
	} else if (in_bits <= SLOW5_BYTES_TO_BITS(sizeof (uint64_t))) {
		for (i = 0; i < n / sizeof (uint64_t); i++) {
			((uint64_t *) be)[i] = slow5_letobe64(((uint64_t *) h)[i]);
		}
	}
}

/* copy be from big endian into h as host endian */
static void uintx_betoh(uint8_t out_bits, const uint8_t *be, uint8_t *h, uint64_t n)
{
	uint64_t i;

	if (out_bits <= SLOW5_BYTES_TO_BITS(sizeof (uint8_t))) {
		return;
	} else if (out_bits <= SLOW5_BYTES_TO_BITS(sizeof (uint16_t))) {
		for (i = 0; i < n / sizeof (uint16_t); i++) {
			((uint16_t *) h)[i] = slow5_be16tole(((uint16_t *) be)[i]);
		}
	} else if (out_bits <= SLOW5_BYTES_TO_BITS(sizeof (uint32_t))) {
		for (i = 0; i < n / sizeof (uint32_t); i++) {
			((uint32_t *) h)[i] = slow5_be32tole(((uint32_t *) be)[i]);
		}
	} else if (out_bits <= SLOW5_BYTES_TO_BITS(sizeof (uint64_t))) {
		for (i = 0; i < n / sizeof (uint64_t); i++) {
			((uint64_t *) h)[i] = slow5_be64tole(((uint64_t *) be)[i]);
		}
	}
}


static int uintx_press(uint8_t in_bits, uint8_t out_bits, const uint8_t *in,
		uint64_t nin, uint8_t *out, uint64_t *nout)
{
	int ret;
	uint64_t nin_bytes;
	uint8_t *in_be;

	if (!out_bits) {
		*nout = 0;
		return 0;
	}

	nin_bytes = nin * SLOW5_BITS_TO_BYTES(in_bits);
	in_be = malloc(nin_bytes);
	if (!in_be)
		return -1;
	uintx_htobe(in_bits, in, in_be, nin_bytes);

	ret = uintx_press_core(in_bits, out_bits, in_be, nin, out, nout);
	free(in_be);

	return ret;
}


static int uint0_depress(uint8_t out_bits, uint64_t nin, uint8_t *out, uint64_t *nout)
{
	uint64_t nout_bytes;

	nout_bytes = nin * SLOW5_BITS_TO_BYTES(out_bits);
	if (*nout < nout_bytes)
		return -1;

	(void) memset(out, 0, nout_bytes);
	*nout = nout_bytes;

	return 0;
}



static int uintx_depress(uint8_t in_bits, uint8_t out_bits, const uint8_t *in,
		  uint64_t nin, uint8_t *out, uint64_t *nout)
{
	int ret;
	uint8_t *out_be;

	if (!in_bits)
		return uint0_depress(out_bits, nin, out, nout);

	out_be = malloc(*nout);
	if (!out_be)
		return -1;

	ret = uintx_press_core(in_bits, out_bits, in, nin, out_be, nout);

	if (ret == 0)
		uintx_betoh(out_bits, out_be, out, *nout);

	free(out_be);
	return ret;
}

#define SLOW5_DEFINE_UINTX(bits) \
static uint64_t uintx_bound_##bits(uint8_t out_bits, uint64_t nin) \
{ \
	return uintx_bound(bits, out_bits, nin); \
} \
static int uintx_press_##bits(uint8_t out_bits, const uint8_t *in, uint64_t nin, \
		       uint8_t *out, uint64_t *nout) \
{ \
	return uintx_press(bits, out_bits, in, nin, out, nout); \
} \
static int uintx_depress_##bits(uint8_t in_bits, const uint8_t *in, uint64_t nin, \
			 uint8_t *out, uint64_t *nout) \
{ \
	return uintx_depress(in_bits, bits, in, nin, out, nout); \
} \

SLOW5_DEFINE_UINTX(16);
SLOW5_DEFINE_UINTX(32);

static uint8_t uint_get_minbits(uint64_t max)
{
	uint8_t i;

	for (i = 0; i <= SLOW5_BYTES_TO_BITS(sizeof max); i++) {
		if (max < pow(2, i))
			return i;
	}

	return SLOW5_BYTES_TO_BITS(sizeof max);
}

static uint8_t uint_get_minbits_32(const uint32_t *in, uint64_t nin)
{
	uint32_t max;

	max = get_max_u32(in, nin);
	return uint_get_minbits(max);
}

uint64_t uint_bound_32(uint8_t out_bits, uint64_t nin)
{
	uint64_t nin_bytes;
	nin_bytes = nin * sizeof (uint32_t);

	return sizeof out_bits + uintx_bound_32(out_bits, nin_bytes);
}

static int uint_press_32(uint8_t out_bits, const uint32_t *in, uint64_t nin,
		  uint8_t *out, uint64_t *nout)
{
	int ret;
	uint64_t nout_uintx;
	const uint8_t *in_uintx;
	uint8_t *out_uintx;

	in_uintx = (const uint8_t *) in;

	out[0] = out_bits;
	out_uintx = out + 1;
	nout_uintx = *nout - sizeof *out;

	ret = uintx_press_32(out_bits, in_uintx, nin, out_uintx, &nout_uintx);

	*nout = 1 + nout_uintx;
	return ret;
}

static int uint_depress_32(const uint8_t *in, uint64_t nin, uint32_t *out,
		    uint64_t *nout)
{
	int ret;
	uint64_t nout_uintx;
	const uint8_t *in_uintx;
	uint8_t *out_uintx;
	uint8_t in_bits;

	in_bits = in[0];
	in_uintx = in + 1;

	out_uintx = (uint8_t *) out;
	nout_uintx = *nout * sizeof *out;

	ret = uintx_depress_32(in_bits, in_uintx, nin, out_uintx, &nout_uintx);

	*nout = nout_uintx / sizeof *out;
	return ret;
}

static uint16_t get_max_u16(const uint16_t *in, uint64_t nin)
{
	uint16_t max;
	uint64_t i;

	max = 0;

	for (i = 0; i < nin; i++) {
		if (in[i] > max)
			max = in[i];
	}

	return max;
}

static uint8_t uint_get_minbits_16(const uint16_t *in, uint64_t nin)
{
	uint16_t max;

	max = get_max_u16(in, nin);
	return uint_get_minbits(max);
}

static uint64_t uint_bound_16(uint8_t out_bits, uint64_t nin)
{
	uint64_t nin_bytes;
	nin_bytes = nin * sizeof (uint16_t);

	return sizeof out_bits + uintx_bound_16(out_bits, nin_bytes);
}

static int uint_press_16(uint8_t out_bits, const uint16_t *in, uint64_t nin,
		  uint8_t *out, uint64_t *nout)
{
	int ret;
	uint64_t nout_uintx;
	const uint8_t *in_uintx;
	uint8_t *out_uintx;

	in_uintx = (const uint8_t *) in;

	out[0] = out_bits;
	out_uintx = out + 1;
	nout_uintx = *nout - sizeof *out;

	ret = uintx_press_16(out_bits, in_uintx, nin, out_uintx, &nout_uintx);

	*nout = 1 + nout_uintx;
	return ret;
}

static int uint_depress_16(const uint8_t *in, uint64_t nin, uint16_t *out,
		    uint64_t *nout)
{
	int ret;
	uint64_t nout_uintx;
	const uint8_t *in_uintx;
	uint8_t *out_uintx;
	uint8_t in_bits;

	in_bits = in[0];
	in_uintx = in + 1;

	out_uintx = (uint8_t *) out;
	nout_uintx = *nout * sizeof *out;

	ret = uintx_depress_16(in_bits, in_uintx, nin, out_uintx, &nout_uintx);

	*nout = nout_uintx / sizeof *out;
	return ret;
}


static void ex_press(const uint16_t *in, uint32_t nin, uint8_t *out,
		  uint64_t *nout)
{
	uint32_t nex;
	uint16_t *ex;
	uint8_t *ex_press;
	uint32_t *ex_pos;
	uint32_t *ex_pos_delta;
	uint8_t *ex_pos_press;
	uint8_t minbits;
	uint64_t nr_press_tmp;
	uint32_t nex_pos_press;
	uint32_t nex_press;
	uint32_t i;
	uint32_t j;
	uint64_t offset;

	nex = 0;
    size_t ex_pos_buff_s = UINT16_MAX;
	ex_pos = malloc(ex_pos_buff_s * sizeof *ex_pos);
	ex = malloc(ex_pos_buff_s * sizeof *ex);

	for (i = 0; i < nin; i++) {
		if (in[i] > UINT8_MAX) {
			ex_pos[nex] = i;
			ex[nex] = in[i] - UINT8_MAX - 1;
			nex++;
			if (nex == 0){
				SLOW5_ERROR("error: ex too many exceptions %d",nex);
            } else if (nex == ex_pos_buff_s) {
                ex_pos_buff_s *= 2;
                ex_pos = realloc(ex_pos, ex_pos_buff_s * sizeof *ex_pos);
                ex = realloc(ex, ex_pos_buff_s * sizeof *ex);
            }
		}
	}

	(void) memcpy(out, &nex, sizeof nex);
	offset = sizeof nex;

	if (nex > 1) {
		ex_pos_delta = delta_increasing_u32(ex_pos, nex);

		minbits = uint_get_minbits_32(ex_pos_delta, nex);
		nr_press_tmp = uint_bound_32(minbits, nex);
		ex_pos_press = malloc(nr_press_tmp);
		/* TODO don't ignore return */
		(void) uint_press_32(minbits, ex_pos_delta, nex, ex_pos_press,
				     &nr_press_tmp);
		free(ex_pos_delta);
		nex_pos_press = (uint32_t) nr_press_tmp;
		/*nex_pos_press = bitnd1pack32(ex_pos, nex, ex_pos_press);*/

		(void) memcpy(out + offset, &nex_pos_press, sizeof nex_pos_press);
		offset += sizeof nex_pos_press;

		if (sizeof nex_pos_press + nex_pos_press > nex * sizeof *ex_pos) {
			fprintf(stderr, "ex_pos bitpack (%ld) > standard (%ld)\n",
					sizeof nex_pos_press + nex_pos_press, nex * sizeof *ex_pos);
		}

		/*
		(void) memcpy(out + offset, ex_pos, nex * sizeof *ex_pos);
		offset += nex * sizeof *ex_pos;
		*/
		(void) memcpy(out + offset, ex_pos_press, nex_pos_press);
		free(ex_pos_press);
		offset += nex_pos_press;

		minbits = uint_get_minbits_16(ex, nex);
		nr_press_tmp = uint_bound_16(minbits, nex);
		ex_press = malloc(nr_press_tmp);
		(void) uint_press_16(minbits, ex, nex, ex_press, &nr_press_tmp);
		nex_press = (uint32_t) nr_press_tmp;

		(void) memcpy(out + offset, &nex_press, sizeof nex_press);
		offset += sizeof nex_press;

		if (sizeof nex_press + nex_press > nex * sizeof *ex) {
			fprintf(stderr, "ex bitpack (%ld) > standard (%ld)\n",
					sizeof nex_press + nex_press, nex * sizeof *ex);
		}

		(void) memcpy(out + offset, ex_press, nex_press);
		free(ex_press);
		offset += nex_press;
	} else if (nex == 1) {
		(void) memcpy(out + offset, ex_pos, nex * sizeof *ex_pos);
		offset += nex * sizeof *ex_pos;
		(void) memcpy(out + offset, ex, nex * sizeof *ex);
		offset += nex * sizeof *ex;
	}
	free(ex);

	j = 0;
	for (i = 0; i < nin; i++) {
		if (j < nex && i == ex_pos[j]) {
			j++;
		} else {
			(void) memcpy(out + offset, in + i, 1);
			offset++;
		}
	}

	free(ex_pos);
	*nout = offset;
}


static void undelta_inplace_increasing_u32(uint32_t *in, uint64_t nin)
{
	uint32_t prev;
	uint64_t i;

	prev = in[0];

	for (i = 1; i < nin; i++) {
		in[i] += prev + 1;
		prev = in[i];
	}
}


void ex_depress(const uint8_t *in, uint64_t nin, uint16_t *out, uint32_t *nout)
{
	uint32_t nex;
	uint16_t *ex;
	uint32_t *ex_pos;
	uint8_t *ex_pos_press;
	uint8_t *ex_press;
	uint32_t nex_press;
	uint32_t nex_pos_press;
	uint64_t nex_pos_delta;
	uint64_t nex_cp;
	uint32_t i;
	uint32_t j;
	uint64_t offset;

	(void) memcpy(&nex, in, sizeof nex);
	offset = sizeof nex;
	ex_pos = malloc(nex * sizeof *ex_pos);

	if (nex > 0) {
		ex = malloc(nex * sizeof *ex);

		if (nex > 1) {
			/*
			(void) memcpy(ex_pos, in + offset, nex * sizeof *ex_pos);
			offset += nex * sizeof *ex_pos;
			*/
			(void) memcpy(&nex_pos_press, in + offset, sizeof nex_pos_press);
			offset += sizeof nex_pos_press;

			ex_pos_press = malloc(nex_pos_press);
			(void) memcpy(ex_pos_press, in + offset, nex_pos_press);
			offset += nex_pos_press;

			/* TODO don't ignore return */
			nex_pos_delta = nex;
            //fprintf(stderr, "nex_pos_delta = %ld\n", nex_pos_delta);
			(void) uint_depress_32(ex_pos_press, nex, ex_pos, &nex_pos_delta);

			free(ex_pos_press);
			undelta_inplace_increasing_u32(ex_pos, nex);

			/*(void) bitnd1unpack32(ex_pos_press, nex_pos_press, ex_pos);*/

			(void) memcpy(&nex_press, in + offset, sizeof nex_press);
			offset += sizeof nex_press;

			ex_press = malloc(nex_press);
			(void) memcpy(ex_press, in + offset, nex_press);
			offset += nex_press;

			nex_cp = nex;
			(void) uint_depress_16(ex_press, nex, ex, &nex_cp);
			free(ex_press);
		} else if (nex == 1) {
			(void) memcpy(ex_pos, in + offset, nex * sizeof *ex_pos);
			offset += nex * sizeof *ex_pos;
			(void) memcpy(ex, in + offset, nex * sizeof *ex);
			offset += nex * sizeof *ex;
		}

		for (i = 0; i < nex; i++) {
			out[ex_pos[i]] = ex[i] + UINT8_MAX + 1;
		}
		free(ex);
	}

	i = 0;
	j = 0;
	while (offset < nin || j < nex) {
		if (j < nex && i == ex_pos[j]) {
			j++;
		} else {
			out[i] = in[offset];
			offset++;
		}

		i++;
	}

	free(ex_pos);
	*nout = i;
}

/*
 * delta | zigzag | ex
 * store first signal at start before vb
 */

static inline uint16_t zigzag_one_16(int16_t x)
{
	return (x + x) ^ (x >> 15);
}

/* return zigzag delta of in with nin elements */
static uint16_t *zigdelta_16_u16(const int16_t *in, uint64_t nin)
{
	int16_t prev;
	uint16_t *out;
	uint64_t i;

	out = malloc(nin * sizeof *out);
	prev = 0;

	for (i = 0; i < nin; i++) {
		out[i] = zigzag_one_16(in[i] - prev);
		prev = in[i];
	}

	return out;
}

static void ex_zd_press_16(const int16_t *in, uint32_t nin, uint8_t *out,
			uint64_t *nout)
{
	uint16_t *in_zd;
	uint64_t nout_tmp;

	in_zd = zigdelta_16_u16(in, nin);

	(void) memcpy(out, in_zd, sizeof *in_zd);
	nout_tmp = *nout - sizeof *in_zd;
	ex_press(in_zd + 1, nin - 1, out + sizeof *in_zd, &nout_tmp);

	*nout = nout_tmp + sizeof *in_zd;
	free(in_zd);
}

static inline int16_t unzigzag_one_16(uint16_t x)
{
	return (x >> 1) ^ -(x & 1);
}

static void unzigdelta_u16_16(const uint16_t *in, uint64_t nin, int16_t *out)
{
	int16_t prev;
	uint64_t i;

	prev = 0;
	for (i = 0; i < nin; i++) {
		out[i] = prev + unzigzag_one_16(in[i]);
		prev = out[i];
	}
}
static void ex_zd_depress_16(const uint8_t *in, uint64_t nin, int16_t *out,
			  uint32_t *nout)
{
	uint16_t *out_zd;
	uint32_t nout_tmp;

	out_zd = malloc(nin * sizeof *out_zd);
	(void) memcpy(out_zd, in, sizeof *out_zd);

	nout_tmp = nin - 1;
	ex_depress(in + sizeof *out_zd, nin - sizeof *out_zd, out_zd + 1,
		       &nout_tmp);
	*nout = nout_tmp + 1;

	unzigdelta_u16_16(out_zd, *nout, out);
	free(out_zd);
}


//memory inefficient - fix this
static uint64_t ex_bound(uint64_t nin)
{
	return sizeof (uint8_t) + SLOW5_EX_MAX_EXCEPTIONS * (sizeof (uint32_t) + 2) + nin - SLOW5_EX_MAX_EXCEPTIONS;
}

static inline size_t ex_zd_bound_16(uint64_t nin)
{
	return sizeof (uint16_t) + ex_bound(nin - 1);
}

static uint8_t *ptr_compress_ex_zd(const int16_t *ptr, size_t count, size_t *n) {

    uint64_t nin = count / sizeof *ptr;
    int16_t *in = (int16_t *) malloc(nin * sizeof *in);
    if (!in) {
        SLOW5_MALLOC_ERROR();
        slow5_errno = SLOW5_ERR_MEM;
        return NULL;
    }
    memcpy(in, ptr, nin * sizeof *in);

	uint64_t nout_vb = ex_zd_bound_16(nin);
    uint8_t *out_vb = (uint8_t *) malloc(nout_vb * sizeof *out_vb + sizeof(uint64_t)); //uint64_ for number of signal elements, do it propoerly
    if (!out_vb) {
        SLOW5_MALLOC_ERROR();
        free(in);
        slow5_errno = SLOW5_ERR_MEM;
        return NULL;
    }

    memcpy(out_vb, &nin, sizeof(uint64_t));


    //error checkkk
	ex_zd_press_16(in, nin, out_vb + sizeof(uint64_t), &nout_vb);
    free(in);
    *n = nout_vb+sizeof(uint64_t);

    return out_vb;

}

static int16_t *ptr_depress_ex_zd(const uint8_t *ptr, size_t count, size_t *n){

    uint64_t nout_1;
    memcpy(&nout_1, ptr, sizeof(uint64_t));
    uint32_t nout = nout_1; //check overflowww

    int16_t *out=malloc(nout*sizeof *out);

	ex_zd_depress_16(ptr+sizeof(uint64_t), count-sizeof(uint64_t), out, &nout);
    SLOW5_ASSERT(nout==nout_1);
	*n = nout * sizeof *out;

	return out;
}

/* Decompress a zlib-compressed string
 *
 * @param       compressed string
 * @param       ptr to size of compressed string, updated to size of returned malloced string
 * @return      malloced string
 */
/*
unsigned char *z_inflate_buf(const char *comp_str, size_t *n) {

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = *n;
    strm.next_in = (Bytef *) comp_str;

    *n = 0;

    uLong prev_sz = 0;
    uLong out_sz = 16328;
    unsigned char *out = (unsigned char *) malloc(sizeof *out * out_sz);
    SLOW5_MALLOC_CHK(out);

    int ret = inflateInit2(&strm, ZLIB_WBITS);

    if (ret != Z_OK) {
        free(out);
        return NULL;
    }

    do {
        strm.avail_out = out_sz;
        strm.next_out = out + prev_sz;

        ret = inflate(&strm, Z_NO_FLUSH);
        SLOW5_ASSERT(ret != Z_STREAM_ERROR);

        switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                free(out);
                (void) inflateEnd(&strm);
                return NULL;
        }


        unsigned have = out_sz - strm.avail_out;
        prev_sz += have;

        if (strm.avail_out == 0) {
            out = (unsigned char *) realloc(out, sizeof *out * (prev_sz + out_sz));
            SLOW5_MALLOC_CHK(out);
        }

    } while (strm.avail_out == 0);

    *n = prev_sz;
    (void) inflateEnd(&strm);

    return out;
}

size_t z_deflate_buf(z_streamp strmp, const void *ptr, uLong size, FILE *f_out, int flush, int *err) {
unsigned char *z_inflate_buf(const char *comp_str, size_t *n) {
    size_t written = 0;

    strmp->avail_in = size;
    strmp->next_in = (Bytef *) ptr;

    uLong out_sz = SLOW5_ZLIB_COMPRESS_CHUNK;
    unsigned char *out = (unsigned char *) malloc(sizeof *out * out_sz);
    SLOW5_MALLOC_CHK(out);

    do {
        strmp->avail_out = out_sz;
        strmp->next_out = out;

        ret = deflate(strmp, flush);
        if (ret == Z_STREAM_ERROR) {
            ERROR("deflate failed\n%s", ""); // testing
            *err = ret;
            return written;
        }

        unsigned have = out_sz - strmp->avail_out;
        size_t tmp;
        if ((tmp = fwrite(out, sizeof *out, have, f_out)) != have || ferror(f_out)) {
            ERROR("fwrite\n%s", ""); // testing
            *err = Z_ERRNO;
            written += tmp * sizeof *out;
            return written;
        }
        written += tmp * sizeof *out;

    } while (strmp->avail_out == 0);

    free(out);
    out = NULL;

    // If still input to deflate
    if (strmp->avail_in != 0) {
        ERROR("still more input to deflate\n%s", ""); // testing
        *err = Z_ERRNO;
    }

    *err = Z_OK;
    return written;
}
*/
