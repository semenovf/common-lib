#!/bin/sh

COMMAND_UTF16LE="iconv -f UTF8 -t UTF16LE"
COMMAND_UTF16BE="iconv -f UTF8 -t UTF16BE"

cwd=`pwd`

for u8 in utf8/data/*.txt ; do
    u16=`basename $u8 .txt`
    $COMMAND_UTF16LE $u8 -o utf16le/data/$u16.txt;
    $COMMAND_UTF16BE $u8 -o utf16be/data/$u16.txt;

    # To exclude prefixed variable naming 'xxd' must be invoked
    # from source directory
    cd $cwd/utf16le/data
    xxd -i $u16.txt > ../$u16.c

    cd $cwd/utf16be/data
    xxd -i $u16.txt > ../$u16.c

   cd $cwd
done



