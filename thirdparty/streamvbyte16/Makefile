# compilation options
# NO_SIMD=1 for SSE SIMD
# ZSTD=1 for zstd

CC = cc
OBJ = streamvbyte16.o
STATICLIB = libstreamvbyte16.a
CFLAGS += -Wall -O2 -std=c99 -fpic -g

ifeq ($(no_simd),1)
else
CFLAGS += -march=native -DVBZ_ENABLE_SIMD
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
