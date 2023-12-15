#pragma once

#include "common.hpp"
#include "decode_scalar.hpp"
#include "intrinsics.hpp"
#include "shuffle_tables.hpp"
#include "svb16.h"  // svb16_key_length

#include <stddef.h>
#include <stdint.h>

#ifdef SVB16_X64

__attribute__ ((__target__ ("ssse3")))
inline __m128i zigzag_decode_x64(__m128i val)
{
    return _mm_xor_si128(
        // N >> 1
        _mm_srli_epi16(val, 1),
        // 0xFFFF if N & 1 else 0x0000
        _mm_srai_epi16(_mm_slli_epi16(val, 15), 15)
        // alternative: _mm_sign_epi16(ones, _mm_slli_epi16(buf, 15))
    );
}

__attribute__ ((__target__ ("ssse3")))
static inline __m128i unpack(uint32_t key, uint8_t const * SVB_RESTRICT * data)
{
    uint8_t const len = (uint8_t)(8 + svb16_popcount(key));
    __m128i data_reg = _mm_loadu_si128((__m128i const *)(*data));
    __m128i const shuffle = *(__m128i const *)(&g_decode_shuffle_table[key]);

    data_reg = _mm_shuffle_epi8(data_reg, shuffle);
    *data += len;

    return data_reg;
}

__attribute__ ((__target__ ("ssse3")))
static inline void store_8(bool UseDelta, bool UseZigzag, int16_t * to, __m128i value, __m128i * prev)
{
    SVB16_IF_CONSTEXPR(UseZigzag) { value = zigzag_decode_x64(value); }

    SVB16_IF_CONSTEXPR(UseDelta)
    {
        __m128i const broadcast_last_16 =
            m128i_from_bytes(14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15);
        // value == [A B C D E F G H] (16 bit values)
        __m128i add = _mm_slli_si128(value, 2);
        // add   == [- A B C D E F G]
        *prev = _mm_shuffle_epi8(*prev, broadcast_last_16);
        // *prev == [P P P P P P P P]
        value = _mm_add_epi16(value, add);
        // value == [A AB BC CD DE FG GH]
        add = _mm_slli_si128(value, 4);
        // add   == [- - A AB BC CD DE EF]
        value = _mm_add_epi16(value, add);
        // value == [A AB ABC ABCD BCDE CDEF DEFG EFGH]
        add = _mm_slli_si128(value, 8);
        // add   == [- - - - A AB ABC ABCD]
        value = _mm_add_epi16(value, add);
        // value == [A AB ABC ABCD ABCDE ABCDEF ABCDEFG ABCDEFGH]
        value = _mm_add_epi16(value, *prev);
        // value == [PA PAB PABC PABCD PABCDE PABCDEF PABCDEFG PABCDEFGH]
        *prev = value;
    }

    _mm_storeu_si128((__m128i *)(to), value);
}

__attribute__ ((__target__ ("sse4.1")))
uint8_t const * decode_sse(bool UseDelta, bool UseZigzag,
    int16_t *out_span,
    uint32_t const out_span_count,
    uint8_t const *keys_span,
    uint32_t const keys_span_count,
    uint8_t const *data_span,
    uint64_t const data_span_count,
    int16_t prev)
{
    // this code treats all input as uint16_t (except the zigzag code, which treats it as int16_t)
    // this isn't a problem, as the scalar code does the same

    int16_t *out = out_span;
    uint32_t const count = out_span_count;
    uint8_t const *keys_it = keys_span;
    uint8_t const *data = data_span;

    // handle blocks of 32 values
    if (count >= 64) {
        uint32_t const key_bytes = count / 8;

        __m128i prev_reg;
        SVB16_IF_CONSTEXPR(UseDelta) { prev_reg = _mm_set1_epi16(prev); }

        int64_t offset = -(int64_t)(key_bytes) / 8 + 1;  // 8 -> 4?
        uint64_t const * keyPtr64 = (uint64_t const *)(keys_it) - offset;
        uint64_t nextkeys;
        memcpy(&nextkeys, keyPtr64 + offset, sizeof(nextkeys));

        __m128i data_reg;

        for (; offset != 0; ++offset) {
            uint64_t keys = nextkeys;
            memcpy(&nextkeys, keyPtr64 + offset + 1, sizeof(nextkeys));
            // faster 16-bit delta since we only have 8-bit values
            if (!keys) {  // 64 1-byte ints in a row

                // _mm_cvtepu8_epi16: SSE4.1
                data_reg =
                    _mm_cvtepu8_epi16(_mm_lddqu_si128((__m128i const *)(data)));
                store_8(UseDelta, UseZigzag, out, data_reg, &prev_reg);
                data_reg =
                    _mm_cvtepu8_epi16(_mm_lddqu_si128((__m128i const *)(data + 8)));
                store_8(UseDelta, UseZigzag, out + 8, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                    _mm_lddqu_si128((__m128i const *)(data + 16)));
                store_8(UseDelta, UseZigzag, out + 16, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                    _mm_lddqu_si128((__m128i const *)(data + 24)));
                store_8(UseDelta, UseZigzag, out + 24, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                    _mm_lddqu_si128((__m128i const *)(data + 32)));
                store_8(UseDelta, UseZigzag, out + 32, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                    _mm_lddqu_si128((__m128i const *)(data + +40)));
                store_8(UseDelta, UseZigzag, out + 40, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                    _mm_lddqu_si128((__m128i const *)(data + 48)));
                store_8(UseDelta, UseZigzag, out + 48, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                    _mm_lddqu_si128((__m128i const *)(data + 56)));
                store_8(UseDelta, UseZigzag, out + 56, data_reg, &prev_reg);
                out += 64;
                data += 64;
                continue;
            }

            data_reg = unpack(keys & 0x00FF, &data);
            store_8(UseDelta, UseZigzag, out, data_reg, &prev_reg);
            data_reg = unpack((keys & 0xFF00) >> 8, &data);
            store_8(UseDelta, UseZigzag, out + 8, data_reg, &prev_reg);

            keys >>= 16;
            data_reg = unpack((keys & 0x00FF), &data);
            store_8(UseDelta, UseZigzag, out + 16, data_reg, &prev_reg);
            data_reg = unpack((keys & 0xFF00) >> 8, &data);
            store_8(UseDelta, UseZigzag, out + 24, data_reg, &prev_reg);

            keys >>= 16;
            data_reg = unpack((keys & 0x00FF), &data);
            store_8(UseDelta, UseZigzag, out + 32, data_reg, &prev_reg);
            data_reg = unpack((keys & 0xFF00) >> 8, &data);
            store_8(UseDelta, UseZigzag, out + 40, data_reg, &prev_reg);

            keys >>= 16;
            data_reg = unpack((keys & 0x00FF), &data);
            store_8(UseDelta, UseZigzag, out + 48, data_reg, &prev_reg);

            // Note we load at least sizeof(__m128i) bytes from the end of data
            // here, need to ensure that is available to read.
            //
            // But we might not use it all depending on the unpacking.
            //
            // This is ok due to `decode_input_buffer_padding_byte_count` enuring
            // extra space on the input buffer.
            data_reg = unpack((keys & 0xFF00) >> 8, &data);
            store_8(UseDelta, UseZigzag, out + 56, data_reg, &prev_reg);

            out += 64;
        }
        {
            uint64_t keys = nextkeys;
            // faster 16-bit delta since we only have 8-bit values
            if (!keys) {  // 64 1-byte ints in a row
                data_reg =
                    _mm_cvtepu8_epi16(_mm_lddqu_si128((__m128i const *)(data)));
                store_8(UseDelta, UseZigzag, out, data_reg, &prev_reg);
                data_reg =
                    _mm_cvtepu8_epi16(_mm_lddqu_si128((__m128i const *)(data + 8)));
                store_8(UseDelta, UseZigzag, out + 8, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                    _mm_lddqu_si128((__m128i const *)(data + 16)));
                store_8(UseDelta, UseZigzag, out + 16, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                    _mm_lddqu_si128((__m128i const *)(data + 24)));
                store_8(UseDelta, UseZigzag, out + 24, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                    _mm_lddqu_si128((__m128i const *)(data + 32)));
                store_8(UseDelta, UseZigzag, out + 32, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                    _mm_lddqu_si128((__m128i const *)(data + +40)));
                store_8(UseDelta, UseZigzag, out + 40, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                    _mm_lddqu_si128((__m128i const *)(data + 48)));
                store_8(UseDelta, UseZigzag, out + 48, data_reg, &prev_reg);
                // Only load the first 8 bytes here, otherwise we may run off the end of the buffer
                data_reg = _mm_cvtepu8_epi16(
                    _mm_loadl_epi64((__m128i const *)(data + 56)));
                store_8(UseDelta, UseZigzag, out + 56, data_reg, &prev_reg);
                out += 64;
                data += 64;

            } else {
                data_reg = unpack(keys & 0x00FF, &data);
                store_8(UseDelta, UseZigzag, out, data_reg, &prev_reg);
                data_reg = unpack((keys & 0xFF00) >> 8, &data);
                store_8(UseDelta, UseZigzag, out + 8, data_reg, &prev_reg);

                keys >>= 16;
                data_reg = unpack((keys & 0x00FF), &data);
                store_8(UseDelta, UseZigzag, out + 16, data_reg, &prev_reg);
                data_reg = unpack((keys & 0xFF00) >> 8, &data);
                store_8(UseDelta, UseZigzag, out + 24, data_reg, &prev_reg);

                keys >>= 16;
                data_reg = unpack((keys & 0x00FF), &data);
                store_8(UseDelta, UseZigzag, out + 32, data_reg, &prev_reg);
                data_reg = unpack((keys & 0xFF00) >> 8, &data);
                store_8(UseDelta, UseZigzag, out + 40, data_reg, &prev_reg);

                keys >>= 16;
                data_reg = unpack((keys & 0x00FF), &data);
                store_8(UseDelta, UseZigzag, out + 48, data_reg, &prev_reg);
                data_reg = unpack((keys & 0xFF00) >> 8, &data);
                store_8(UseDelta, UseZigzag, out + 56, data_reg, &prev_reg);

                out += 64;
            }
        }
        prev = out[-1];

        keys_it += key_bytes - (key_bytes & 7);
    }

    assert(out <= out_span + out_span_count);
    assert(keys_it <= keys_span + keys_span_count);
    assert(data <= data_span + data_span_count);

    assert(out_span + out_span_count - out == (count & 63));

    uint32_t const out_scalar_count = out_span_count - (out - out_span);
    uint32_t const keys_scalar_count = keys_span_count - (keys_it - keys_span);
    uint64_t const data_scalar_count = data_span_count - (data - data_span);

    return decode_scalar(UseDelta, UseZigzag,
        out, out_scalar_count, keys_it, keys_scalar_count, data, data_scalar_count, prev);
}

#endif  // SVB16_X64
