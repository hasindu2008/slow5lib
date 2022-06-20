cmake_minimum_required(VERSION 3.5)

project(streamvbyte_slow5)
#todo add processor specific flags

EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
message( STATUS "Architecture: ${ARCHITECTURE}" )

option(SVB_NO_SIMD "Don't use simd" OFF) #OFF by default
if(SVB_NO_SIMD)
	message( STATUS "SVB compilation without SIMD" )
	set(CMAKE_C_FLAGS " -fPIC -std=c99 -O3 -Wall -Wextra -pedantic -Wshadow ")
else()
	if( ${ARCHITECTURE} STREQUAL "x86_64" )
		set(CMAKE_C_FLAGS " -fPIC -std=c99 -O3 -Wall -Wextra -pedantic -Wshadow -DSTREAMVBYTE_SSSE3=1 -mssse3 ")
	elseif( ${ARCHITECTURE} STREQUAL "aarch64" )
		set(CMAKE_C_FLAGS " -fPIC -std=c99 -O3 -Wall -Wextra -pedantic -Wshadow -D__ARM_NEON__ ")
	elseif( ${ARCHITECTURE} STREQUAL "armv7l" )
		set(CMAKE_C_FLAGS " -fPIC -std=c99 -O3 -Wall -Wextra -pedantic -Wshadow -mfpu=neon ")
	else()
		set(CMAKE_C_FLAGS " -fPIC -std=c99 -O3 -Wall -Wextra -pedantic -Wshadow ")
	endif()
endif(SVB_NO_SIMD)
unset(SVB_NO_SIMD CACHE)

set(streamvbyte_decode src/streamvbyte_decode.c)
set(streamvbyte_encode src/streamvbyte_encode.c)
set(streamvbyte_zigzag src/streamvbyte_zigzag.c)

# Build a shared lib
add_library(streamvbyte_slow5 STATIC ${streamvbyte_decode} ${streamvbyte_encode} ${streamvbyte_zigzag})
