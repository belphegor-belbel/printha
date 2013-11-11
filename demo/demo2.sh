#!/bin/sh
printha --init
printha --import csvoid.txt < addressbook.csv > 2/addressbook.pdf
rm printha.config.txt sendto.txt sendfrom.txt

