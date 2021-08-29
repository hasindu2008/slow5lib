# minimalist makefile
.SUFFIXES:
#
.SUFFIXES: .cpp .o .c .h

PROCESSOR:=$(shell uname -m)

ifeq ($(PROCESSOR), aarch64)
# for 64-bit ARM processors
CFLAGS = -fPIC -std=c99 -O3 -Wall -Wextra -pedantic -Wshadow -D__ARM_NEON__
else ifeq ($(PROCESSOR), armv7l)
# for 32-bit ARM processors
CFLAGS = -fPIC -std=c99 -O3 -Wall -Wextra -pedantic -Wshadow
else ifeq ($(PROCESSOR), x86_64)
# Here we expect x64
# Formally speaking, we only need SSE4, at best, but code checks for AVX
# since MSVC only allows to check for AVX and nothing finer like just SSE4
CFLAGS = -fPIC -march=native -std=c99 -O3 -Wall -Wextra -pedantic -Wshadow
else
CFLAGS = -fPIC -mtune=native -std=c99 -O3 -Wall -Wextra -pedantic -Wshadow
endif
LDFLAGS = -shared
LIBNAME=libstreamvbyte.so.0.0.1
LNLIBNAME=libstreamvbyte.so
STATICLIB=libstreamvbyte.a
all:  $(LIBNAME) $(STATICLIB)

HEADERS=./include/streamvbyte.h ./include/streamvbytedelta.h ./include/streamvbyte_zigzag.h

OBJECTS= streamvbyte_decode.o streamvbyte_encode.o streamvbytedelta_decode.o streamvbytedelta_encode.o streamvbyte_0124_encode.o  streamvbyte_0124_decode.o streamvbyte_zigzag.o

streamvbyte_zigzag.o: ./src/streamvbyte_zigzag.c $(HEADERS)
	$(CC) $(CFLAGS) -c ./src/streamvbyte_zigzag.c -Iinclude


streamvbytedelta_encode.o: ./src/streamvbytedelta_encode.c $(HEADERS)
	$(CC) $(CFLAGS) -c ./src/streamvbytedelta_encode.c -Iinclude

streamvbytedelta_decode.o: ./src/streamvbytedelta_decode.c $(HEADERS)
	$(CC) $(CFLAGS) -c ./src/streamvbytedelta_decode.c -Iinclude

streamvbyte_0124_encode.o: ./src/streamvbyte_0124_encode.c $(HEADERS)
	$(CC) $(CFLAGS) -c ./src/streamvbyte_0124_encode.c -Iinclude

streamvbyte_0124_decode.o: ./src/streamvbyte_0124_decode.c $(HEADERS)
	$(CC) $(CFLAGS) -c ./src/streamvbyte_0124_decode.c -Iinclude

streamvbyte_decode.o: ./src/streamvbyte_decode.c $(HEADERS)
	$(CC) $(CFLAGS) -c ./src/streamvbyte_decode.c -Iinclude

streamvbyte_encode.o: ./src/streamvbyte_encode.c $(HEADERS)
	$(CC) $(CFLAGS) -c ./src/streamvbyte_encode.c -Iinclude

$(LIBNAME): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(LIBNAME) $(OBJECTS)  $(LDFLAGS)

$(STATICLIB): $(OBJECTS)
	ar rcs $@ $^

$(LNLIBNAME): $(LIBNAME)
	ln -f -s $(LIBNAME) $(LNLIBNAME)


clean:
	rm -f unit *.o $(LIBNAME) $(LNLIBNAME) $(STATICLIB)
