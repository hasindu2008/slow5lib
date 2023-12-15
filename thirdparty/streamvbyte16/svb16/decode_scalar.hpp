#pragma once

#include "common.hpp"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

inline uint16_t zigzag_decode(uint16_t val)
{
    return (val >> 1) ^ (uint16_t)(0 - (val & 1));
}

inline uint16_t decode_data(uint8_t const ** dataPtr, uint8_t code)
{
    uint16_t val;

    if (code == 0) {  // 1 byte
        val = (uint16_t)(**dataPtr);
        *dataPtr += 1;
    } else {  // 2 bytes
        val = 0;
        memcpy(&val, *dataPtr, 2);  // assumes little endian
        *dataPtr += 2;
    }

    return val;
}

uint8_t const * decode_scalar(bool UseDelta, bool UseZigzag,
    int16_t *out_span,
    uint32_t out_span_size,
    uint8_t const *keys_span,
    uint32_t keys_span_size,
    uint8_t const *data_span,
    uint64_t data_span_size,
    int16_t prev)
{
    uint32_t const count = out_span_size;
    if (count == 0) {
        return data_span;
    }

    int16_t *out = out_span;
    uint8_t const *keys = keys_span;
    uint8_t const *data = data_span;

    uint8_t shift = 0;  // cycles 0 through 7 then resets
    uint8_t key_byte = *keys++;
    // need to do the arithmetic in unsigned space so it wraps
    uint16_t u_prev = (uint16_t)(prev);
    for (uint32_t c = 0; c < count; c++, shift++) {
        if (shift == 8) {
            shift = 0;
            key_byte = *keys++;
        }
        uint16_t value = decode_data(&data, (key_byte >> shift) & 0x01);
        SVB16_IF_CONSTEXPR(UseZigzag) { value = zigzag_decode(value); }
        SVB16_IF_CONSTEXPR(UseDelta)
        {
            value += u_prev;
            u_prev = value;
        }
        *out++ = (int16_t)(value);
    }

    assert(out == out_span + out_span_size);
    assert(keys == keys_span + keys_span_size);
    assert(data <= data_span + data_span_size);
    return data;
}
