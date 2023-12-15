#pragma once

#include "common.hpp"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

inline uint16_t zigzag_encode(uint16_t val)
{
    return (val + val) ^ (uint16_t)((int16_t)(val) >> 15);
}

uint8_t * encode_scalar(bool UseDelta, bool UseZigzag,
    int16_t const * in,
    uint8_t * SVB_RESTRICT keys,
    uint8_t * SVB_RESTRICT data,
    uint32_t count,
    int16_t prev)
{
    if (count == 0) {
        return data;
    }

    uint8_t shift = 0;  // cycles 0 through 7 then resets
    uint8_t key_byte = 0;
    for (uint32_t c = 0; c < count; c++) {
        if (shift == 8) {
            shift = 0;
            *keys++ = key_byte;
            key_byte = 0;
        }
        uint16_t value;
        SVB16_IF_CONSTEXPR(UseDelta)
        {
            // need to do the arithmetic in unsigned space so it wraps
            value = (uint16_t)(in[c]) - (uint16_t)(prev);
            SVB16_IF_CONSTEXPR(UseZigzag) { value = zigzag_encode(value); }
            prev = in[c];
        }
        else SVB16_IF_CONSTEXPR(UseZigzag) {
            value = zigzag_encode((uint16_t)(in[c]));
        }
        else {
            value = (uint16_t)(in[c]);
        }

        if (value < (1 << 8)) {  // 1 byte
            *data = (uint8_t)(value);
            ++data;
        } else {                           // 2 bytes
            memcpy(data, &value, 2);  // assumes little endian
            data += 2;
            key_byte |= 1 << shift;
        }

        shift += 1;
    }

    *keys = key_byte;  // write last key (no increment needed)
    return data;
}
