# compilation options
# no_simd=1 for no SSE SIMD
# zstd=1 for zstd

CC = cc
OBJ = streamvbyte16.o
STATICLIB = libstreamvbyte16.a
CFLAGS += -fPIC -std=c99 -O3 -Wall -Wextra -pedantic -Wshadow

PROC_AUTO:=$(shell uname -m)
PROCESSOR ?= $(PROC_AUTO)

ifeq ($(no_simd),1)
else
CFLAGS += -DVBZ_ENABLE_SIMD
ifeq ($(PROCESSOR), aarch64)
# for 64-bit ARM processors
CFLAGS += -D__ARM_NEON__
else ifeq ($(PROCESSOR), armv7l)
# for 32-bit ARM processors
CFLAGS += -mfpu=neon
else ifeq ($(PROCESSOR), x86_64)
CFLAGS += -msse4.1
endif
endif

ifeq ($(zstd),1)
CFLAGS += -DVBZ_USE_ZSTD
#LDFLAGS += -lzstd
endif

.PHONY = clean

$(STATICLIB): $(OBJ)
	$(AR) rcs $@ $^

streamvbyte16.o: streamvbyte16.c streamvbyte16.h svb16/*.h
	$(CC) $(CFLAGS) $< -c -fpic -o $@

clean:
	rm -f $(OBJ) $(STATICLIB)
