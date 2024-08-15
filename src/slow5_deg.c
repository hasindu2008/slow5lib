/*
 * Irreversible signal degradation (lossy compression)
 * Aug 15 2024
 * Sasha Jenner (me AT sjenner DOT com)
 */

#include "slow5_deg.h"

static int round_to_power_of_2(int number, int number_of_bits);

/*
 * Return a suggestion for the number of bits to use with qts degradation given
 * a slow5 file. Return 0 on error and set slow5_errno.
 */
int8_t slow5_suggest_qts(const struct slow5_file *p)
{
    if (!p) {
        SLOW5_ERROR("Argument '%s' cannot be NULL.", SLOW5_TO_STR(p));
        slow5_errno = SLOW5_ERR_ARG;
        return 0;
    }

    return 0;
}

/*
 * Zero number_of_bits LSB of number by rounding it to the nearest power of 2.
 * number_of_bits must be >= 1. Return the rounded number.
 * Taken from https://github.com/hasindu2008/sigtk src/qts.c.
 */
static int round_to_power_of_2(int number, int number_of_bits) {
    //create a binary mask with the specified number of bits
    int bit_mask = (1 << number_of_bits) - 1;
    
    //extract out the value of the LSBs considered
    int lsb_bits = number & bit_mask;
    
     
    int round_threshold = (1 << (number_of_bits - 1));
    
    //check if the least significant bits are closer to 0 or 2^n
    if (lsb_bits < round_threshold) {
        return (number & ~bit_mask) + 0; //round down to the nearest power of 2
    } else {
        return (number & ~bit_mask) + (1 << number_of_bits); //round up to the nearest power of 2
    }
}

/*
 * For array a with length n, zero b LSB of each integer by rounding them to the
 * nearest power of 2. Set slow5_errno on error.
 */
void slow5_arr_qts_round(int16_t *a, uint64_t n, uint8_t b)
{
    uint64_t i;

    if (!b)
        return;
    if (!a) {
        SLOW5_ERROR("Argument '%s' cannot be NULL.", SLOW5_TO_STR(a));
        slow5_errno = SLOW5_ERR_ARG;
        return;
    }

    for (i = 0; i < n; i++)
        a[i] = round_to_power_of_2(a[i], (int) b);
}

/*
 * For the signal array in a slow5 record, zero b LSB of each integer by
 * rounding them to the nearest power of 2. Set slow5_errno on error.
 */
void slow5_rec_qts_round(struct slow5_rec *r, uint8_t b)
{
    if (!r) {
        SLOW5_ERROR("Argument '%s' cannot be NULL.", SLOW5_TO_STR(r));
        slow5_errno = SLOW5_ERR_ARG;
    } else {
        slow5_arr_qts_round(r->raw_signal, r->len_raw_signal, b);
    }
}
