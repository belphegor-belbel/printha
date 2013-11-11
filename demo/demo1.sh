#!/bin/sh
printha --init
printha < sendto.txt > 1/sendto.pdf
printha --svg < sendto.txt > 1/sendto.svg
printha --ps < sendto.txt > 1/sendto.ps
printha --import $(dirname $0)/../settings/config.company.txt < sendto.txt > 1/president.pdf
printha --import $(dirname $0)/../settings/config.telephone.txt< sendto.txt > 1/telephone.pdf
printha --import nenga.txt < sendto.txt > 1/nenga.pdf
rm printha.config.txt sendto.txt sendfrom.txt

