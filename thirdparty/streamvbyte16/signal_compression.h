#ifndef SIGNAL_COMPRESSION_H
#define SIGNAL_COMPRESSION_H

#include <stdint.h>

/*
 * Return the maximum VBZ compressed size given the number of 16-bit samples
 * sample_count.
 */
size_t compressed_signal_max_size(size_t sample_count);

/*
 * VBZ compress count samples and return the result with byte length *n.
 * n cannot be NULL. Responsibility for freeing the return value is passed onto
 * the caller.
 */
uint8_t *compress_signal(
        const int16_t *samples,
        uint32_t count,
        size_t *n);

/*
 * Decompress VBZ-compressed compressed_bytes with count bytes and return the
 * result with array length *n. Responsibility for freeing the return value is
 * passed onto the caller.
 */
int16_t *decompress_signal(
        const uint8_t *compressed_bytes,
        size_t count,
        uint32_t *n);

/*
 * Same as compress_signal but without the final zstd compression layer. This is
 * essentially just 16-bit streamvbyte zigzag delta accelerated using SSE SIMD.
 * Responsibility for freeing the return value is passed onto the caller.
 */
uint8_t *compress_signal_nozstd(
        const int16_t *samples,
        uint32_t count,
        size_t *n);

/*
 * Same as decompress_signal but without the final zstd compression layer. This
 * is essentially just 16-bit streamvbyte zigzag delta accelerated using
 * SSE/SIMD. Responsibility for freeing the return value is passed onto the
 * caller.
 */
int16_t *decompress_signal_nozstd(
        const uint8_t *compressed_bytes,
        size_t count,
        uint32_t *n);

#endif /* signal_compression.h */
