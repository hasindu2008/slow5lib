cmake_minimum_required(VERSION 3.14)

# choose the architecture
set(DEPLOY_PLATFORM "unknown" CACHE STRING "choose the instruction set architecture")
set_property(CACHE DEPLOY_PLATFORM PROPERTY STRINGS unknown x86 arm64-v8a armeabi-v7a)
IF (${DEPLOY_PLATFORM} STREQUAL "unknown")
    message(FATAL_ERROR "choose the DEPLOY_PLATFORM")
    return() # This is to stop proceeding further and to stop opencv getting set to the default ANDROID_ABI
ENDIF()

project(slow5lib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2 -std=gnu99 -fopenmp")

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/test)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/streamvbyte/include)

set(slow5_ src/slow5.c)
set(slow5_idx src/slow5_idx.c)
set(slow5_misc src/slow5_misc.c)
set(slow5_press src/slow5_press.c)

# example source files
set(random_read examples/random_read.c)
set(sequential_read examples/sequential_read.c)
set(slow5_hdr_get examples/get_hdr_attribute.c)
set(slow5_aux_get examples/get_aux_field.c)
set(pthread_get_reads examples/random_read_pthreads.c)
set(openmp_get_reads examples/random_read_openmp.c)
add_executable(slow5exmp ${openmp_get_reads})

add_subdirectory(thirdparty/streamvbyte)

# Build a static lib
#add_library(slow5 STATIC ${slow5_} ${slow5_idx} ${slow5_misc} ${slow5_press})
#
# Build a shared lib
add_library(slow5 SHARED ${slow5_} ${slow5_idx} ${slow5_misc} ${slow5_press})
target_link_libraries(slow5 streamvbyte_slow5)
#
#just to get rid of Clion warnings
file(GLOB_RECURSE C_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "test/*.c")
add_executable(slow5test test/unit_test_two_rg.c) #run make test to test all tests in one go. this is mostly for debugging
#
#
# link with dependencies
IF (${DEPLOY_PLATFORM} STREQUAL "x86")
    target_link_libraries(slow5test slow5 streamvbyte_slow5 -lz -ldl -lm -lpthread -lrt)
    target_link_libraries(slow5exmp slow5 streamvbyte_slow5 -lz -ldl -lm -lpthread -lrt -fopenmp)
ELSE()
    target_link_libraries(slow5test slow5 streamvbyte_slow5 -lz -ldl -lm)
    target_link_libraries(slow5exmp slow5 streamvbyte_slow5 -lz -ldl -lm)
ENDIF()
