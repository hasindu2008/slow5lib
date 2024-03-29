#!/bin/sh

if [ "$1" = 'mem' ]; then
    mem=1
else
    mem=0
fi

echo_test() {
    printf '\n--%s--\n' "$1"
}

ex() {
    if [ $mem -eq 1 ]; then
        valgrind --leak-check=full --error-exitcode=1 "$@"
    else
        "$@"
    fi
}

fail() {
    echo 'FAILURE'
    ret=1
}

my_diff() {
    if ! diff "$1" "$2" -q; then
        fail
    fi
}

prep() {
    mkdir -p 'test/bin'
    mkdir -p 'test/data/out/one_fast5'
    mkdir -p 'test/data/out/two_rg'
    mkdir -p 'test/data/out/aux_array'
}

prep_unit() {
    rm test/data/out/*.idx
    rm test/data/out/one_fast5/*
    rm test/data/out/two_rg/*
    rm test/data/out/aux_array/*
    rm test/data/exp/one_fast5/*.idx
    rm test/data/exp/two_rg/*.idx
    rm test/data/exp/aux_array/*.idx
    rm test/data/test/*.idx
    rm test/data/err/*.idx

    cp 'test/data/exp/one_fast5/exp_1_default.slow5' 'test/data/out/exp_1_default_add_empty.slow5'
    cp 'test/data/exp/one_fast5/exp_1_default.slow5' 'test/data/out/exp_1_default_add_valid.slow5'
    cp 'test/data/exp/one_fast5/exp_1_default.slow5' 'test/data/out/exp_1_default_add_duplicate.slow5'
}

ret=0

prep

echo_test 'endian test'
ex test/bin/endian_test

prep_unit

echo_test 'unit test helpers'
if ! ex test/bin/unit_test_helpers; then
    fail
fi

echo_test 'unit test press'
if ! ex test/bin/unit_test_press > test/data/out/unit_test_out_press; then
    fail
fi

echo_test 'unit test ascii'
if ! ex test/bin/unit_test_ascii > test/data/out/unit_test_out_ascii; then
    fail
fi

echo_test 'unit test binary'
if ! ex test/bin/unit_test_binary > test/data/out/unit_test_out_binary; then
    fail
fi

echo_test 'slow5 conversion test'
if ! ex test/bin/convert_slow5_test; then
    fail
fi

echo_test 'unit test two read groups'
if ! ex test/bin/unit_test_two_rg > test/data/out/unit_test_out_two_rg; then
    fail
fi

echo_test 'unit test lossless'
if ! ex test/bin/unit_test_lossless > test/data/out/unit_test_out_lossless; then
    fail
fi

echo_test 'unit test empty'
if ! ex test/bin/unit_test_empty > test/data/out/unit_test_out_empty; then
    fail
fi

echo_test 'unit test enum'
if ! ex test/bin/unit_test_enum > test/data/out/unit_test_out_enum; then
    fail
fi


echo_test 'diff test'
# Unit test output diffs
my_diff 'test/data/out/unit_test_out_ascii' 'test/data/exp/unit_test_exp_ascii'
my_diff 'test/data/out/unit_test_out_fprint' 'test/data/exp/unit_test_exp_fprint'
my_diff 'test/data/out/unit_test_out_binary' 'test/data/exp/unit_test_exp_binary'
my_diff 'test/data/out/unit_test_out_press' 'test/data/exp/unit_test_exp_press'
my_diff 'test/data/out/unit_test_out_lossless' 'test/data/exp/unit_test_exp_lossless'
my_diff 'test/data/out/unit_test_out_empty' 'test/data/exp/unit_test_exp_empty'
my_diff 'test/data/out/unit_test_out_two_rg' 'test/data/exp/unit_test_exp_two_rg'
my_diff 'test/data/out/unit_test_out_enum' 'test/data/exp/unit_test_exp_enum'
# Adding records diffs
my_diff 'test/data/out/exp_1_default_add_empty.slow5' 'test/data/exp/exp_1_default_add_empty.slow5'
my_diff 'test/data/out/exp_1_default_add_valid.slow5' 'test/data/exp/exp_1_default_add_valid.slow5'
my_diff 'test/data/out/exp_1_default_add_duplicate.slow5' 'test/data/exp/exp_1_default_add_duplicate.slow5'
## Conversion diffs
# Default
my_diff 'test/data/out/one_fast5/slow5_to_blow5_uncomp.blow5' 'test/data/exp/one_fast5/exp_1_default.blow5'
my_diff 'test/data/out/one_fast5/slow5_to_blow5_gzip.blow5' 'test/data/exp/one_fast5/exp_1_default_gzip.blow5'
my_diff 'test/data/out/one_fast5/blow5_uncomp_to_slow5.slow5' 'test/data/exp/one_fast5/exp_1_default.slow5'
my_diff 'test/data/out/one_fast5/blow5_gzip_to_slow5.slow5' 'test/data/exp/one_fast5/exp_1_default.slow5'
my_diff 'test/data/out/one_fast5/blow5_gzip_to_blow5_uncomp.blow5' 'test/data/exp/one_fast5/exp_1_default.blow5'
my_diff 'test/data/out/one_fast5/blow5_uncomp_to_blow5_gzip.blow5' 'test/data/exp/one_fast5/exp_1_default_gzip.blow5'
# Lossless
my_diff 'test/data/out/one_fast5/slow5_to_blow5_uncomp_lossless.blow5' 'test/data/exp/one_fast5/exp_1_lossless.blow5'
my_diff 'test/data/out/one_fast5/slow5_to_blow5_gzip_lossless.blow5' 'test/data/exp/one_fast5/exp_1_lossless_gzip.blow5'
my_diff 'test/data/out/one_fast5/blow5_uncomp_to_slow5_lossless.slow5' 'test/data/exp/one_fast5/exp_1_lossless.slow5'
my_diff 'test/data/out/one_fast5/blow5_gzip_to_slow5_lossless.slow5' 'test/data/exp/one_fast5/exp_1_lossless.slow5'
my_diff 'test/data/out/one_fast5/blow5_gzip_to_blow5_uncomp_lossless.blow5' 'test/data/exp/one_fast5/exp_1_lossless.blow5'
my_diff 'test/data/out/one_fast5/blow5_uncomp_to_blow5_gzip_lossless.blow5' 'test/data/exp/one_fast5/exp_1_lossless_gzip.blow5'
# Lossless with auxiliary array
my_diff 'test/data/out/aux_array/slow5_to_blow5_uncomp_lossless.blow5' 'test/data/exp/aux_array/exp_lossless.blow5'
my_diff 'test/data/out/aux_array/slow5_to_blow5_gzip_lossless.blow5' 'test/data/exp/aux_array/exp_lossless_gzip.blow5'
my_diff 'test/data/out/aux_array/blow5_uncomp_to_slow5_lossless.slow5' 'test/data/exp/aux_array/exp_lossless.slow5'
my_diff 'test/data/out/aux_array/blow5_gzip_to_slow5_lossless.slow5' 'test/data/exp/aux_array/exp_lossless.slow5'
my_diff 'test/data/out/aux_array/blow5_gzip_to_blow5_uncomp_lossless.blow5' 'test/data/exp/aux_array/exp_lossless.blow5'
my_diff 'test/data/out/aux_array/blow5_uncomp_to_blow5_gzip_lossless.blow5' 'test/data/exp/aux_array/exp_lossless_gzip.blow5'
# Two rg unit test
my_diff 'test/data/out/two_rg/out_default.blow5' 'test/data/exp/two_rg/exp_default.blow5'
my_diff 'test/data/out/two_rg/out_default_gzip.blow5' 'test/data/exp/two_rg/exp_default_gzip.blow5'

exit $ret
