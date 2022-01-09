cmake_minimum_required(VERSION 3.14)

project(streamvbyte_slow5)
#todo add processor specific flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fPIC -march=native -std=c99 -O3 -Wall -Wextra -pedantic -Wshadow")

set(streamvbyte_decode src/streamvbyte_decode.c)
set(streamvbyte_encode src/streamvbyte_encode.c)
set(streamvbyte_zigzag src/streamvbyte_zigzag.c)

# Build a shared lib
add_library(streamvbyte_slow5 STATIC ${streamvbyte_decode} ${streamvbyte_encode} ${streamvbyte_zigzag})
