CC       = gcc
AR 		 = ar
CFLAGS   += -g -Wall -O2
LDFLAGS  +=  -lz
BUILD_DIR = build


OBJ_LIB = $(BUILD_DIR)/slow5.o \
		$(BUILD_DIR)/slow5_idx.o	\
		$(BUILD_DIR)/slow5_misc.o	\
		$(BUILD_DIR)/slow5_press.o \

PREFIX = /usr/local
VERSION = `git describe --tags`

.PHONY: clean distclean format test install uninstall slow5lib

#libslow5
slow5lib: $(BUILD_DIR)/libslow5.so $(BUILD_DIR)/libslow5.a

$(BUILD_DIR)/libslow5.so: $(OBJ_LIB)
	$(CC) $(CFLAGS) -shared $^  -o $@ $(LDFLAGS)

$(BUILD_DIR)/libslow5.a: $(OBJ_LIB)
	$(AR) rcs $@ $^

$(BUILD_DIR)/slow5.o: src/slow5.c src/slow5.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -c -fpic -o $@

$(BUILD_DIR)/slow5_idx.o: src/slow5_idx.c src/slow5_idx.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -c -fpic -o $@

$(BUILD_DIR)/slow5_misc.o: src/slow5_misc.c src/slow5_misc.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -c -fpic -o $@

$(BUILD_DIR)/slow5_press.o: src/slow5_press.c src/slow5_press.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -c -fpic -o $@

clean:
	rm -rf $(BINARY) $(BUILD_DIR)/*.o

# Delete all gitignored files (but not directories)
distclean: clean
	git clean -f -X
	rm -rf $(BUILD_DIR)/* autom4te.cache

dist: distclean
	mkdir -p slow5tools-$(VERSION)
	autoreconf
	cp -r README.md LICENSE Makefile configure.ac config.mk.in \
		installdeps.mk src docs build configure slow5tools-$(VERSION)
	mkdir -p slow5tools-$(VERSION)/scripts
	cp scripts/install-hdf5.sh slow5tools-$(VERSION)/scripts
	tar -zcf slow5tools-$(VERSION)-release.tar.gz slow5tools-$(VERSION)
	rm -rf slow5tools-$(VERSION)

binary:
	mkdir -p slow5tools-$(VERSION)
	make clean
	make && mv slow5tools slow5tools-$(VERSION)/slow5tools_x86_64_linux
	cp -r README.md LICENSE docs slow5tools-$(VERSION)/
	#mkdir -p slow5tools-$(VERSION)/scripts
	#cp scripts/test.sh slow5tools-$(VERSION)/scripts
	tar -zcf slow5tools-$(VERSION)-binaries.tar.gz slow5tools-$(VERSION)
	rm -rf slow5tools-$(VERSION)

install: $(BINARY)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1
	cp -f $(BINARY) $(DESTDIR)$(PREFIX)/bin
	gzip < docs/slow5tools.1 > $(DESTDIR)$(PREFIX)/share/man/man1/slow5tools.1.gz

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(BINARY) \
		$(DESTDIR)$(PREFIX)/share/man/man1/slow5tools.1.gz

test: $(slow5lib)
	./test/test.sh

pyslow5:
	make clean
	rm -rf *.so python/pyslow5.cpp build/lib.* build/temp.*
	python3 setup.py build
	cp build/lib.*/*.so  ./
	python3 < python/example.py

test-prep: $(BINARY)
	gcc test/make_blow5.c -Isrc src/slow5.c src/slow5_press.c -lz src/slow5_idx.c src/slow5_misc.c -o test/bin/make_blow5 -g
	./test/bin/make_blow5

valgrind: $(BINARY)
	./test/test.sh mem

examples: slow5lib
	./examples/build.sh
