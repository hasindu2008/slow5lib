# compilation options
# NO_SIMD=1 for SSE SIMD
# ZSTD=1 for zstd

CC = cc
OBJ = streamvbyte16.o
STATICLIB = libstreamvbyte16.a
CFLAGS += -Wall -O3 -std=c99 -fpic -g

PROC_AUTO:=$(shell uname -m)
PROCESSOR ?= $(PROC_AUTO)

ifeq ($(no_simd),1)
else
ifeq ($(PROCESSOR), aarch64)
CFLAGS += -DVBZ_ENABLE_SIMD
else ifeq ($(PROCESSOR), x86_64)
CFLAGS += -DVBZ_ENABLE_SIMD -msse4.1
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
