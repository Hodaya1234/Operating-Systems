#!/bin/bash
#Hodaya koslowsky
#task number1

cd $1
res=( $(find . -maxdepth 1 -name '*.txt' -type f) )
num=${#res[@]}

echo Number of files in the directory that end with .txt is $num
