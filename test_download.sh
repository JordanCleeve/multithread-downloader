#!/bin/bash


wget -O __template $1 -q
./http_download $1 __output

diff __template __output --report-identical-files --brief
rm __template
rm __output