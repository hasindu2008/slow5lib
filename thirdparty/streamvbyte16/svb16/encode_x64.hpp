#pragma once

#include "common.hpp"
#include "encode_scalar.hpp"
#include "intrinsics.hpp"
#include "shuffle_tables.hpp"
#include "svb16.h"  // svb16_key_length

#include <stddef.h>
#include <stdint.h>

#ifdef SVB16_X64

__attribute__ ((__target__ ("ssse3")))
inline __m128i delta(__m128i curr, __m128i prev)
{
    return _mm_sub_epi16(curr, _mm_alignr_epi8(curr, prev, 14));
}

__attribute__ ((__target__ ("ssse3")))
inline __m128i zigzag_encode_x64(__m128i val)
{
    return _mm_xor_si128(_mm_add_epi16(val, val), _mm_srai_epi16(val, 16));
}

__attribute__ ((__target__ ("ssse3")))
inline __m128i load_8(bool UseDelta, bool UseZigzag, int16_t const * from, __m128i * prev)
{
    __m128i const loaded = _mm_loadu_si128((__m128i const *)(from));
    SVB16_IF_CONSTEXPR(UseDelta && UseZigzag)
    {
        __m128i const result = delta(loaded, *prev);
        *prev = loaded;
        return zigzag_encode_x64(result);
    }
    else SVB16_IF_CONSTEXPR(UseDelta) {
        __m128i const result = delta(loaded, *prev);
        *prev = loaded;
        return result;
    }
    else SVB16_IF_CONSTEXPR(UseZigzag) {
        return zigzag_encode_x64(loaded);
    }
    else {
        return loaded;
    }
}

__attribute__ ((__target__ ("ssse3")))
uint8_t * encode_sse(bool UseDelta, bool UseZigzag,
    int16_t const * in,
    uint8_t * SVB_RESTRICT keys_dest,
    uint8_t * SVB_RESTRICT data_dest,
    uint32_t count,
    int16_t prev)
{
    // this code treats all input as uint16_t (except the zigzag code, which treats it as int16_t)
    // this isn't a problem, as the scalar code does the same
    __m128i prev_reg;
    SVB16_IF_CONSTEXPR(UseDelta) { prev_reg = _mm_set1_epi16(prev); }
    //auto const key_len = svb16_key_length(count);
    __m128i const mask_01 = m128i_from_bytes(
        0x01,
        0x01,
        0x01,
        0x01,
        0x01,
        0x01,
        0x01,
        0x01,
        0x01,
        0x01,
        0x01,
        0x01,
        0x01,
        0x01,
        0x01,
        0x01);
    for (int16_t const * end = &in [(count & ~15)]; in != end; in += 16) {
        // load up 16 values into r0 and r1
        __m128i r0 = load_8(UseDelta, UseZigzag, in, &prev_reg);
        __m128i r1 = load_8(UseDelta, UseZigzag, in + 8, &prev_reg);

        // 1 byte per input byte: 1 if the byte is set, 0 if not
        __m128i r2 = _mm_min_epu8(mask_01, r0);
        __m128i r3 = _mm_min_epu8(mask_01, r1);
        // 1 byte per input Int16T: FF if the MSB is set, 00 or 01 if not
        // (us = unsigned saturation)
        r2 = _mm_packus_epi16(r2, r3);
        // 1 bit per input Int16T: 1 if the MSB is set, 0 if not
        // only the low 16 bits are set
        uint16_t const keys = (uint16_t)(_mm_movemask_epi8(r2));

        // use the shuffle table to discard the MSB if the corresponidng key bit is not set
        r2 = _mm_loadu_si128((__m128i *)&g_encode_shuffle_table[(keys << 4) & 0x07F0]);
        r3 = _mm_loadu_si128((__m128i *)&g_encode_shuffle_table[(keys >> 4) & 0x07F0]);
        r0 = _mm_shuffle_epi8(r0, r2);
        r1 = _mm_shuffle_epi8(r1, r3);

        // store the data to data_dest (note that we often end up with overlapping writes)
        _mm_storeu_si128((__m128i *)(data_dest), r0);
        data_dest += 8 + svb16_popcount(keys & 0xFF);
        _mm_storeu_si128((__m128i *)(data_dest), r1);
        data_dest += 8 + svb16_popcount(keys >> 8);

        *((uint16_t *)keys_dest) = keys;
        keys_dest += 2;
    }

    SVB16_IF_CONSTEXPR(UseDelta) { prev = _mm_extract_epi16(prev_reg, 7); }
    // max two control bytes (16 values) left, use the scalar function
    count &= 15;
    return encode_scalar(UseDelta, UseZigzag, in, keys_dest, data_dest, count, prev);
}

#endif  // SVB16_X64
