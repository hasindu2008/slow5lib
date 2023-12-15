#pragma once

#include "common.hpp"
#include "decode_scalar.hpp"
#include "svb16.h"  // svb16_key_length
#ifdef SVB16_X64
#include "decode_x64.hpp"
#include "simd_detect_x64.hpp"
#endif

// Required extra space after readable buffers passed in.
//
// Require 1 128 bit buffer beyond the end of all input readable buffers.
inline size_t decode_input_buffer_padding_byte_count()
{
#ifdef SVB16_X64
    return sizeof(__m128i);
#else
    return 0;
#endif
}

size_t decode(bool UseDelta, bool UseZigzag, int16_t *out, uint32_t out_size, uint8_t const *in, uint64_t in_size, int16_t prev)
{
    uint32_t keys_length = svb16_key_length(out_size);
    uint64_t data_size = in_size - keys_length;
    uint8_t const *keys = in;
    uint8_t const *data = in + keys_length;
#ifdef SVB16_X64
    if (has_sse4_1()) {
        return decode_sse(UseDelta, UseZigzag, out, out_size, keys, keys_length, data, data_size, prev) - in;
    }
#endif
    return decode_scalar(UseDelta, UseZigzag, out, out_size, keys, keys_length, data, data_size, prev) - in;
}
