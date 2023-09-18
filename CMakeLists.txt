cmake_minimum_required(VERSION 3.5)

# choose the architecture
set(DEPLOY_PLATFORM x86)
# set_property(CACHE DEPLOY_PLATFORM PROPERTY STRINGS unknown x86 arm64-v8a armeabi-v7a)

project(slow5lib)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2 -std=gnu99 -fopenmp")
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O0 -std=c99")
ELSEIF()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2 -std=c99")
ENDIF()

option(SLOW5_USE_ZSTD "Use ZSTD" OFF) #OFF by default
if(SLOW5_USE_ZSTD)
    add_compile_options("-DSLOW5_USE_ZSTD")
    link_libraries("-lzstd")
endif(SLOW5_USE_ZSTD)
unset(SLOW5_USE_ZSTD CACHE)

option(SLOW5_ENABLE_MT "Use SLOW5_ENABLE_MT" OFF) #OFF by default
if(SLOW5_ENABLE_MT)
    add_compile_options("-DSLOW5_ENABLE_MT")
endif(SLOW5_ENABLE_MT)
unset(SLOW5_ENABLE_MT CACHE)

# option(SLOW5_USE_OPENMP "Use OPENMP" OFF) #OFF by default
# if(SLOW5_USE_OPENMP)
#     add_compile_options("-fopenmp")
#     link_libraries("-fopenmp")
# endif(SLOW5_USE_OPENMP)
# unset(SLOW5_USE_OPENMP CACHE)

# include_directories(${PROJECT_SOURCE_DIR}/src)
# include_directories(${PROJECT_SOURCE_DIR}/test)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/streamvbyte/include)

set(slow5_ src/slow5.c)
set(slow5_idx src/slow5_idx.c)
set(slow5_misc src/slow5_misc.c)
set(slow5_press src/slow5_press.c)
set(slow5_mt src/slow5_mt.c)

# example source files
# set(random_read examples/random_read.c)
# set(sequential_read examples/sequential_read.c)
# set(slow5_hdr_get examples/get_hdr_attribute.c)
# set(slow5_aux_get examples/get_aux_field.c)
# set(pthread_get_reads examples/random_read_pthreads.c)
# set(openmp_get_reads examples/random_read_openmp.c)
# add_executable(slow5exmp ${openmp_get_reads})

add_subdirectory(thirdparty/streamvbyte)

option(SLOW5_LINK_STATIC "libslow5 will create a static lib" OFF) #OFF by default
if(SLOW5_LINK_STATIC)
    message( STATUS "libslow5 will create a static lib" )
    add_library(slow5 STATIC ${slow5_} ${slow5_idx} ${slow5_misc} ${slow5_press} ${slow5_mt})
else()
    message( STATUS "libslow5 will create a shared lib" )
    add_library(slow5 SHARED ${slow5_} ${slow5_idx} ${slow5_misc} ${slow5_press} ${slow5_mt})
endif(SLOW5_LINK_STATIC)
unset(SLOW5_LINK_STATIC CACHE)

target_link_libraries(slow5 streamvbyte_slow5)

# Build a static lib
#add_library(slow5 STATIC ${slow5_} ${slow5_idx} ${slow5_misc} ${slow5_press})
#
# Build a shared lib
#
#just to get rid of Clion warnings
# file(GLOB_RECURSE C_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "test/*.c")
# add_executable(slow5test test/unit_test_two_rg.c) #run make test to test all tests in one go. this is mostly for debugging
#
#
# link with dependencies
# IF (${DEPLOY_PLATFORM} STREQUAL "x86")
#     target_link_libraries(slow5test slow5 streamvbyte_slow5 -lz -ldl -lm -lpthread -lrt)
#     target_link_libraries(slow5exmp slow5 streamvbyte_slow5 -lz -ldl -lm -lpthread -lrt)
# ELSE()
#     target_link_libraries(slow5test slow5 streamvbyte_slow5 -lz -ldl -lm)
#     target_link_libraries(slow5exmp slow5 streamvbyte_slow5 -lz -ldl -lm)
# ENDIF()
