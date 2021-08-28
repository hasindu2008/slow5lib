CC			= cc
SRC			= ../src
LIB			= ../lib
CPPFLAGS	+= -I ../include/ -I $(SRC)/
#CFLAGS		+= -g -Wall -Werror -Wpedantic -std=c99
CFLAGS		+= -g -Wall -Werror -std=gnu99
LDFLAGS		+= $(LIB)/libslow5.a -lm -lz
ifeq ($(ZSTD),1)
CFLAGS		+= -DSLOW5_USE_ZSTD
LDFLAGS		+= -lzstd
endif
BIN_DIR		= bin

TESTS = $(BIN_DIR)/endian_test \
	$(BIN_DIR)/unit_test_helpers \
	$(BIN_DIR)/unit_test_press \
	$(BIN_DIR)/unit_test_ascii \
	$(BIN_DIR)/unit_test_binary \
	$(BIN_DIR)/convert_slow5_test \
	$(BIN_DIR)/unit_test_two_rg \
	$(BIN_DIR)/unit_test_lossless \
	$(BIN_DIR)/unit_test_empty

all: $(BIN_DIR) $(TESTS)

$(BIN_DIR):
	mkdir $@

$(BIN_DIR)/endian_test: endian_test.c $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/unit_test_helpers: unit_test_helpers.c $(BIN_DIR) unit_test.h $(LIB)/libslow5.a $(SRC)/slow5_extra.h $(SRC)/slow5_misc.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/unit_test_press: unit_test_press.c $(BIN_DIR) unit_test.h $(LIB)/libslow5.a
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/unit_test_ascii: unit_test_ascii.c $(BIN_DIR) unit_test.h $(LIB)/libslow5.a $(SRC)/slow5_extra.h $(SRC)/slow5_misc.h $(SRC)/slow5_idx.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/unit_test_binary: unit_test_binary.c $(BIN_DIR) unit_test.h $(LIB)/libslow5.a $(SRC)/slow5_extra.h $(SRC)/slow5_misc.h $(SRC)/slow5_idx.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/convert_slow5_test: convert_slow5_test.c $(BIN_DIR) unit_test.h $(LIB)/libslow5.a
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/unit_test_two_rg: unit_test_two_rg.c $(BIN_DIR) unit_test.h $(LIB)/libslow5.a $(SRC)/slow5_extra.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/unit_test_lossless: unit_test_lossless.c $(BIN_DIR) unit_test.h $(LIB)/libslow5.a $(SRC)/slow5_extra.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/unit_test_empty: unit_test_empty.c $(BIN_DIR) unit_test.h $(LIB)/libslow5.a $(SRC)/slow5_extra.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -rf $(TESTS) $(BIN_DIR)
