#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "signal_compression.h"

#include "svb16/decode.hpp"
#include "svb16/encode.hpp"

#ifdef VBZ_USE_ZSTD
#include <zstd.h>
#endif /* VBZ_USE_ZSTD */

/*
 * Return the maximum VBZ compressed size given the number of 16-bit samples
 * sample_count.
 */
size_t compressed_signal_max_size(size_t sample_count) {
#ifdef VBZ_USE_ZSTD
    const uint32_t max_svb_size = svb16_max_encoded_length(sample_count);
    const size_t zstd_compressed_max_size = ZSTD_compressBound(max_svb_size);
    return zstd_compressed_max_size;
#else
    return 0;
#endif /* VBZ_USE_ZSTD */
}

/*
 * VBZ compress count samples and return the result with byte length *n.
 * n cannot be NULL. Responsibility for freeing the return value is passed onto
 * the caller.
 */
uint8_t *compress_signal(
        const int16_t *samples,
        uint32_t count, /* signal count shouldn't exceed 2^32 - 1 */
        size_t *n)
{
#ifdef VBZ_USE_ZSTD
    // First compress the data using svb:
    const uint32_t max_size = svb16_max_encoded_length(count) + sizeof count;
    uint8_t *intermediate = malloc(max_size);

    /* I realise that the samples count is stored elsewhere and given to the
     * original vbz depress algorithm, but we are going to store it with the
     * compressed data for ease and flexibility. */
    (void) memcpy(intermediate, &count, sizeof count);

    bool UseDelta = true;
    bool UseZigzag = true;
    const size_t encoded_count = encode(UseDelta, UseZigzag,
            samples, intermediate + sizeof count, count, 0);

    // Now compress the svb data using zstd:
    size_t const zstd_compressed_max_size = ZSTD_compressBound(encoded_count + sizeof count);
    if (ZSTD_isError(zstd_compressed_max_size)) {
        /* Failed to find zstd max size for data */
        free(intermediate);
        return NULL;
    }

    /* Compress.
     * If you are doing many compressions, you may want to reuse the context.
     * See the multiple_simple_compression.c example.
     */
    uint8_t *destination = malloc(zstd_compressed_max_size);
    /*
     * TODO
     * The compression level is set to 1 but it should be higher, preferably
     * SLOW5_ZSTD_COMPRESS_LEVEL. But I'm leaving it for now to mimic pod5.
     */
    const size_t compressed_size = ZSTD_compress(destination, zstd_compressed_max_size,
                                                 intermediate, encoded_count + sizeof count, 1);
    free(intermediate);
    if (ZSTD_isError(compressed_size)) {
        /* Failed to compress data */
        free(destination);
        return NULL;
    }

    *n = compressed_size;
    return destination;
#else
    *n = 0;
    return NULL;
#endif /* VBZ_USE_ZSTD */
}

/*
 * Decompress VBZ-compressed compressed_bytes with count bytes and return the
 * result with array length *n. Responsibility for freeing the return value is
 * passed onto the caller.
 */
int16_t *decompress_signal(
        const uint8_t *compressed_bytes,
        size_t count,
        uint32_t *n)
{
#ifdef VBZ_USE_ZSTD
    // First decompress the data using zstd:
    unsigned long long const decompressed_zstd_size =
            ZSTD_getFrameContentSize(compressed_bytes, count);
    if (ZSTD_isError(decompressed_zstd_size)) {
        /* Input data not compressed by zstd */
        return NULL;
    }

    size_t allocation_padding = decode_input_buffer_padding_byte_count();

    uint64_t intermediate_size = decompressed_zstd_size + allocation_padding;
    uint8_t *intermediate = malloc(intermediate_size);
    size_t const decompress_res =
            ZSTD_decompress(intermediate, intermediate_size,
                            compressed_bytes, count);
    if (ZSTD_isError(decompress_res)) {
        /* Input data failed to compress using zstd */
        free(intermediate);
        return NULL;
    }

    (void) memcpy(n, intermediate, sizeof *n);
    int16_t *destination = malloc(sizeof *destination * (*n));

    // Now decompress the data using svb:
    bool UseDelta = true;
    bool UseZigzag = true;
    size_t consumed_count = decode(UseDelta, UseZigzag,
            destination, *n, intermediate + sizeof *n, decompress_res - sizeof *n, 0);

    free(intermediate);
    if (consumed_count != decompress_res - sizeof *n) {
        /* Remaining data at end of signal buffer */
        free(destination);
        return NULL;
    }

    return destination;
#else
    *n = 0;
    return NULL;
#endif /* VBZ_USE_ZSTD */
}

/*
 * Same as compress_signal but without the final zstd compression layer. This is
 * essentially just 16-bit streamvbyte zigzag delta accelerated using SSE SIMD.
 * Responsibility for freeing the return value is passed onto the caller.
 */
uint8_t *compress_signal_nozstd(
        const int16_t *samples,
        uint32_t count, /* signal count shouldn't exceed 2^32 - 1 */
        size_t *n)
{
    // First compress the data using svb:
    const uint32_t max_size = svb16_max_encoded_length(count) + sizeof count;
    uint8_t *intermediate = malloc(max_size);

    /* I realise that the samples count is stored elsewhere and given to the
     * original vbz depress algorithm, but we are going to store it with the
     * compressed data for ease and flexibility. */
    (void) memcpy(intermediate, &count, sizeof count);

    bool UseDelta = true;
    bool UseZigzag = true;
    const size_t encoded_count = encode(UseDelta, UseZigzag,
            samples, intermediate + sizeof count, count, 0);

    *n = encoded_count + sizeof count;
    return intermediate;
}

/*
 * Same as decompress_signal but without the final zstd compression layer. This
 * is essentially just 16-bit streamvbyte zigzag delta accelerated using
 * SSE/SIMD. Responsibility for freeing the return value is passed onto the
 * caller.
 */
int16_t *decompress_signal_nozstd(
        const uint8_t *compressed_bytes,
        size_t count,
        uint32_t *n)
{
    (void) memcpy(n, compressed_bytes, sizeof *n);
    int16_t *destination = malloc(sizeof *destination * (*n));

    // Now decompress the data using svb:
    bool UseDelta = true;
    bool UseZigzag = true;
    size_t consumed_count = decode(UseDelta, UseZigzag,
            destination, *n, compressed_bytes + sizeof *n, count - sizeof *n, 0);

    if (consumed_count != count - sizeof *n) {
        /* Remaining data at end of signal buffer */
        free(destination);
        return NULL;
    }

    return destination;
}
