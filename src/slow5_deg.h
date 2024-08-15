/*
 * Irreversible signal degradation (lossy compression) API
 * Aug 15 2024
 * Sasha Jenner (me AT sjenner DOT com)
 */

#ifndef SLOW5_DEG
#define SLOW5_DEG

#include <stdint.h>
#include <slow5/slow5.h>
#include <slow5/slow5_error.h>

#ifdef __cplusplus
extern "C" {
#endif

extern enum slow5_log_level_opt slow5_log_level;
extern enum slow5_exit_condition_opt slow5_exit_condition;

/*
 * Return a suggestion for the number of bits to use with qts degradation given
 * a slow5 file. Return 0 on error and set slow5_errno.
 */
int8_t slow5_suggest_qts(const struct slow5_file *p);

/*
 * For array a with length n, zero b LSB of each integer by rounding them to the
 * nearest power of 2. Set slow5_errno on error.
 */
void slow5_arr_qts_round(int16_t *a, uint64_t n, uint8_t b);

/*
 * For the signal array in a slow5 record, zero b LSB of each integer by
 * rounding them to the nearest power of 2. Set slow5_errno on error.
 */
void slow5_rec_qts_round(struct slow5_rec *r, uint8_t b);

#ifdef __cplusplus
}
#endif

#endif /* slow5_deg.h */
