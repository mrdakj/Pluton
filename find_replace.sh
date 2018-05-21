#!/bin/zsh

find=$1
replace=$2

echo `egrep -r $find src/*.cpp include/*.hpp -oh | wc -l`
echo $find
echo $replace

sed_str='s/'$find'/'$replace'/g'

for x in src/*.cpp include/*.hpp;do sed -e $sed_str $x > $x.tmp && mv $x.tmp $x;done

