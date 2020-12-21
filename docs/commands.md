# Commands and Options

## COMMANDS 

* `f2s`:               
         Converts from FAST5 to SLOW5/BLOW5.
* `s2f`:    
         Converts from SLOW5/BLOW5 to FAST5.
* `index`:           
         Indexes a SLOW5/BLOW5 file.
* `view`:          
         SLOW5<->BLOW5 conversion and subsetting.   
* `cat`:          
         Concatenate SLOW5/BLOW5 files.
* `split`:          
         Splits a SLOW5/BLOW5 file.
* `stats`:
         Generates Statistics from a SAM/BAM file.


## OPTIONS

### f2s

`slow5tools f2s [OPTIONS] fast5_dir1 fast5dir2 ... >  out.slow5`

Recursively searches for FAST5 files (.fast5 extension) in specified directories and converts them to SLOW5/BLOW5 format.

*  `b`, `--binary`:
   Outputs BLOW5.
*  `-c INT`, `--compress INT`:
   Outputs compressed BLOW5 at compression level specified by INT (compression levels 1 to 9 as in gzip). This option implies -b.
*  `-h`, `--help`:                           
   Prints the help to the standard out.
*  `-i FILE`, `--index FILE`
   Generates SLOW5/BLOW5 index.
*  `-o FILE`, `--output FILE`:
   Outputs converted contents to FILE [default value: stdout]
*  `-p, --iop INT`:
    Number of I/O processes [default value: 8]. Increasing the number of I/O processes makes conversion significantly faster, especially on HPC with RAID systems (multiple disks) where this can be as high as 64. 
*   `--lossy`:
    Discard useless information in FAST5.
*  `--no-merge DIR`:
    Convert each FAST5 file to a separate SLOW5/BLOW5 and write to the directory specified by DIR. -o is ineffective with this option.
*  `--no-recursion`:
    Do not recursively search for FAST5 files in specified directories.   
*  `--tmp-prefix` STR:
    Write temporary files to STR.nnnn.blow5 [default value: tmp]
*  `--verbose INT`:
    Verbosity level for the log messages [default value: 0].
*  `--version`:
    Print the version number to the standard out. 