#ifndef SLOW5_DEFS_H
#define SLOW5_DEFS_H

#include <stdint.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

// SLOW5 format specs
#define SLOW5_HEADER_PREFIX             "#"
#define SLOW5_HEADER_DATA_PREFIX        "@"
#define SLOW5_HEADER_DATA_PREFIX_CHAR   '@'
#define SLOW5_COLUMN_HEADER_PREFIX            "#"
#define SLOW5_SEP_COL                         "\t"
#define SLOW5_SEP_COL_CHAR                    '\t'
#define SLOW5_SEP_ARRAY                       ","
#define SLOW5_SEP_ARRAY_CHAR                  ','
#define SLOW5_HEADER_FILE_VERSION             "slow5_version"
#define SLOW5_HEADER_NUM_GROUPS               "num_read_groups"
#define SLOW5_HEADER_NUM_GROUPS_INIT          (1)

// Order, format string and type of main SLOW5 columns
// NOTE if this is changed, also edit:
//      slow5.c:slow5_rec_to_str SLOW5_FORMAT_BINARY
//      slow5idx_clean.c
#define SLOW5_COLS(col, end)                    \
    col(char*,      "%s",       read_id)        /* A malloced string */ \
    col(uint32_t,   "%" PRIu32, read_group)     \
    col(double,     "%s",       digitisation)   \
    col(double,     "%s",       offset)         \
    col(double,     "%s",       range)          \
    col(double,     "%s",       sampling_rate)  \
    col(uint64_t,   "%" PRIu64, len_raw_signal) \
    end(int16_t*,   ,           raw_signal) // Use end() for last column
// Format string of raw signal
#define SLOW5_FORMAT_STRING_RAW_SIGNAL "%" PRId16

// Apply the same function to each column including the last one
#define SLOW5_COLS_FOREACH(foo) SLOW5_COLS(foo, foo)

#define SLOW5_GENERATE_STRUCT(type, fmt, name)            type name;
#define SLOW5_GENERATE_ENUM(type, fmt, name)              COL_ ## name,
#define SLOW5_GENERATE_NAME_STRING(type, fmt, name)       #name
#define SLOW5_GENERATE_NAME_STRING_SEP(type, fmt, name)   SLOW5_GENERATE_NAME_STRING(type, fmt, name) SLOW5_SEP_COL
#define SLOW5_GENERATE_TYPE_STRING(type, fmt, name)       #type
#define SLOW5_GENERATE_TYPE_STRING_SEP(type, fmt, name)   SLOW5_GENERATE_TYPE_STRING(type, fmt, name) SLOW5_SEP_COL
#define SLOW5_GENERATE_FORMAT_STRING(type, fmt, name)     fmt
#define SLOW5_GENERATE_FORMAT_STRING_SEP(type, fmt, name) SLOW5_GENERATE_FORMAT_STRING(type, fmt, name) SLOW5_SEP_COL
#define SLOW5_GENERATE_NULL(type, fmt, name)

// More SLOW5 specs
#define SLOW5_HEADER_ID(header_name)    SLOW5_HEADER_PREFIX header_name
#define SLOW5_HEADER_FILE_VERSION_ID          SLOW5_HEADER_ID(SLOW5_HEADER_FILE_VERSION)
#define SLOW5_HEADER_NUM_GROUPS_ID            SLOW5_HEADER_ID(SLOW5_HEADER_NUM_GROUPS)

#define SLOW5_HEADER_ENTRY(header_name, data) SLOW5_HEADER_ID(header_name) SLOW5_SEP_COL data "\n"

// ASCII SLOW5 specs
#define SLOW5_ASCII_NAME                      "slow5"
#define SLOW5_ASCII_EXTENSION                 "." SLOW5_ASCII_NAME
#define SLOW5_ASCII_VERSION                   "0.1.0"
#define SLOW5_ASCII_VERSION_FORMAT            "%" PRIu8 ".%" PRIu8 ".%" PRIu8
#define SLOW5_ASCII_NUM_GROUPS_FORMAT         "%" PRIu32
#define SLOW5_ASCII_ENTRY_VERSION             SLOW5_HEADER_ENTRY(SLOW5_HEADER_FILE_VERSION, SLOW5_ASCII_VERSION)
#define SLOW5_ASCII_ENTRY_VERSION_FORMAT      SLOW5_HEADER_ENTRY(SLOW5_HEADER_FILE_VERSION, SLOW5_ASCII_VERSION_FORMAT)
#define SLOW5_ASCII_ENTRY_NUM_GROUPS_FORMAT   SLOW5_HEADER_ENTRY(SLOW5_HEADER_NUM_GROUPS, SLOW5_ASCII_NUM_GROUPS_FORMAT)
#define SLOW5_ASCII_SLOW5_HEADER_FORMAT       SLOW5_ASCII_ENTRY_VERSION_FORMAT SLOW5_ASCII_ENTRY_NUM_GROUPS_FORMAT
#define SLOW5_ASCII_TYPE_HEADER_MIN           SLOW5_COLUMN_HEADER_PREFIX SLOW5_COLS(SLOW5_GENERATE_TYPE_STRING_SEP, SLOW5_GENERATE_TYPE_STRING)
#define SLOW5_ASCII_COLUMN_HEADER_MIN         SLOW5_COLUMN_HEADER_PREFIX SLOW5_COLS(SLOW5_GENERATE_NAME_STRING_SEP, SLOW5_GENERATE_NAME_STRING)

// Binary SLOW5 specs
#define SLOW5_BINARY_NAME                     "blow5"
#define SLOW5_BINARY_EXTENSION                "." SLOW5_BINARY_NAME
#define SLOW5_BINARY_VERSION                  { 0, 1, 0 }
#define SLOW5_BINARY_MAGIC_NUMBER             { 'B', 'L', 'O', 'W', '5', '\1' }
#define SLOW5_BINARY_EOF                      { '5', 'W', 'O', 'L', 'B' }
#define SLOW5_BINARY_HEADER_SIZE_OFFSET       (64L)

// SLOW5 Index specs
//#define SLOW5_INDEX_HEADER_PREFIX   "#"
#define SLOW5_INDEX_HEADER          SLOW5_INDEX_HEADER_PREFIX "read_id" SLOW5_SEP_COL "offset" SLOW5_SEP_COL "length\n"

#ifdef __cplusplus
}
#endif

#endif
