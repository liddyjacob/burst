## SYNOPSIS

`burst` \[OPTIONS\]

## DESCRIPTION

The program `burst` takes a file and splits it into smaller files using threads. The files are given names in the form file-(fileno).ext. 

## OPTIONS

`-l (Lines)`
: Split file into smaller files with (Lines) lines

`-v`
: Verbose: Status and other information in stderr.

## EXAMPLES


Suppose test.txt is 1200 lines

burst test.txt
: Split test.txt into 500 line files, execept the last file, which will contain the leftover (200 in this case) lines.

burst test.txt -l 250
: Do the same thing as above, except splits into 250 line files and a 200 line file.

## BUGS


If a single line exceeds 4096 bytes, then burst will return an error and fail.

Report bugs to jpl61@zips.uakron.edu

## SEE ALSO

  `split`(1)

## AUTHORS

Written by Jacob Liddy, and Michael L. Collard.

## COPYRIGHT

    Copyright  ©  2017  Free Software Foundation, Inc.  License GPLv3+: GNU 
    GPL version 3 or later <http://gnu.org/licenses/gpl.html>. 
    This is free software: you are free  to  change  and  redistribute  it. 
    There is NO WARRANTY, to the extent permitted by law.
