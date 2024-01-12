#pragma once

#include "common.h"
#include "encode_scalar.h"
#include "svb16.h"  // svb16_key_length
#ifdef SVB16_X64
#include "encode_x64.h"
#include "simd_detect_x64.h"
#endif

size_t encode(bool UseDelta, bool UseZigzag, int16_t const *in, uint8_t * SVB_RESTRICT out, uint32_t count, int16_t prev)
{
    uint8_t *keys = out;
    uint8_t *data = keys + svb16_key_length(count);
#ifdef SVB16_X64
    if (has_ssse3()) {
        return encode_sse(UseDelta, UseZigzag, in, keys, data, count, prev) - out;
    }
#endif
    return encode_scalar(UseDelta, UseZigzag, in, keys, data, count, prev) - out;
}
