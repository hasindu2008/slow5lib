cmake_minimum_required(VERSION 3.14)

# choose the architecture
set(DEPLOY_PLATFORM "unknown" CACHE STRING "choose the instruction set architecture")
set_property(CACHE DEPLOY_PLATFORM PROPERTY STRINGS unknown x86 arm64-v8a armeabi-v7a)
IF (${DEPLOY_PLATFORM} STREQUAL "unknown")
    message(FATAL_ERROR "choose the DEPLOY_PLATFORM")
    return() # This is to stop proceeding further and to stop opencv getting set to the default ANDROID_ABI
ENDIF()

project(slow5lib)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fPIC -rdynamic -Wall -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -rdynamic -Wall -O2 -std=c++11 ")
set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/test)

set_source_files_properties(src/klib/kstring.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(src/slow5.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(src/slow5_idx.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(src/slow5_misc.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(src/slow5_press.c PROPERTIES LANGUAGE CXX)

set(kstring src/klib/kstring.c)
set(slow5_ src/slow5.c)
set(slow5_idx src/slow5_idx.c)
set(slow5_misc src/slow5_misc.c)
set(slow5_press src/slow5_press.c)

# Build a static lib
add_library(slow5 STATIC ${kstring} ${slow5_} ${slow5_idx} ${slow5_misc} ${slow5_press})

# Build a shared lib
#add_library(slow5 SHARED ${kstring} ${slow5} ${slow5idx} ${slow5_misc} ${slow5_press})

#just to get rid of Clion warnings
file(GLOB_RECURSE C_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "test/*.c")
add_executable(slow5test test/unit_test_two_rg.c) #run make test to test all tests in one go. this is mostly for debugging

target_include_directories(slow5 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# link with dependencies
IF (${DEPLOY_PLATFORM} STREQUAL "x86")
    target_link_libraries(slow5 -lz -ldl -lm -lpthread -lrt)
    target_link_libraries(slow5test slow5 -lz -ldl -lm -lpthread -lrt)
ELSE()
    target_link_libraries(slow5 -lz -ldl -lm)
    target_link_libraries(slow5test slow5 -lz -ldl -lm)
ENDIF()