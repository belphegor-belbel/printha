#!/bin/sh
printha --init
python csvparser.py < addressbook2.csv | printha > 4/csv.pdf
rm printha.config.txt sendto.txt sendfrom.txt

