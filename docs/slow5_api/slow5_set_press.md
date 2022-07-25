# slow5_set_press

## NAME

slow5_set_press - sets the compression method for a BLOW5 file opened for writing

## SYNOPSYS

`int slow5_set_press(slow5_file_t *s5p, enum slow5_press_method rec_press, enum slow5_press_method sig_press)`

## DESCRIPTION

`slow5_set_press()` sets the record compression and signal compression in a BLOW5 file *s5p* to *rec_press* and *sig_press*, respectively. *s5p* should have been opened for writing (mode "w") using `slow5_open()` and must be a binary SLOW5 (BLOW5) file.
`slow5_set_press()` must be called immediately after calling `slow5_open()`.

*rec_press* can be one of the following:
* `SLOW5_COMPRESS_NONE`		No compression
* `SLOW5_COMPRESS_ZLIB`		Zlib (default)
* `SLOW5_COMPRESS_ZSTD`		Zstandard

*sig_press* can be one of the following:
* `SLOW5_COMPRESS_NONE`		No compression
* `SLOW5_COMPRESS_SVB_ZD`	Stream variable byte - zig-zag delta (default)

## RETURN VALUE

Upon successful completion, `slow5_set_press()` returns a non negative integer (>=0). Otherwise, a negative value is returned that indicates the error and `slow5_errno` is set to indicate the error.

## ERRORS

* `SLOW5_ERR_ARG`
    &nbsp;&nbsp;&nbsp;&nbsp; Invalid argument - s5p is NULL.


## NOTES

If `slow5_set_press()` is not explicitly called, the default will be `SLOW5_COMPRESS_ZLIB` for record compression and `SLOW5_COMPRESS_SVB_ZD` for signal compression.  Default values are recommended for most cases. `SLOW5_COMPRESS_ZSTD`+`SLOW5_COMPRESS_SVB_ZD` is similar to VBZ compression from Oxford Nanopore Technologies.
If `SLOW5_COMPRESS_ZSTD` is specified, *slow5lib* require to be built with zstd support and your programme must be linked with libzstd (-lzstd flag). SLOW5 files compressed with zstd offer smaller file size and better performance compared to the default zlib.
However, zlib runtime library is available by default on almost all distributions unlike zstd and thus files compressed with zlib will be more 'portable'.

## EXAMPLES


No record and signal compression:

```
slow5_file_t *sp = slow5_open(FILE_PATH, "w");
if(sp==NULL){
	fprintf(stderr,"Error opening file!\n");
	exit(EXIT_FAILURE);
}

//set no record compression, no signal compression
if(slow5_set_press(sp, SLOW5_COMPRESS_NONE, SLOW5_COMPRESS_NONE) < 0){ //
	fprintf(stderr,"Error setting compression method!\n");
	exit(EXIT_FAILURE);
}

//... do the rest as in slow5_write example
```

Compression similar to VBZ:

```
slow5_file_t *sp = slow5_open(FILE_PATH, "w");
if(sp==NULL){
	fprintf(stderr,"Error opening file!\n");
	exit(EXIT_FAILURE);
}

//set zstd record compression, svb-zd signal compression
if(slow5_set_press(sp, SLOW5_COMPRESS_ZSTD, SLOW5_COMPRESS_SVB_ZD) < 0){ //
	fprintf(stderr,"Error setting compression method!\n");
	exit(EXIT_FAILURE);
}

//... do the rest as in slow5_write example
```

## SEE ALSO
[slow5_open()](slow5_open.md), [slow5_write()](slow5_write.md)
