#!/bin/sh
printha --init
printf "%s %s様\n%s %s様;%07d;%s\n%s" "織田" "信長" "　　" "市　" 0000000 "滋賀県蒲生郡安土町一丁目" "安土城本丸天守最上階" | printha > 3/renmei.pdf
rm printha.config.txt sendto.txt sendfrom.txt

