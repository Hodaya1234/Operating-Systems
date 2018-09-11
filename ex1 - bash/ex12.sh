#!/bin/bash
#Hodaya koslowsky
#task number2

cd $1
for file in * 
do
	if [[ -d $file ]]; then
	    echo "$file is a directory"
	elif [[ -f $file ]]; then
	    echo "$file is a file"
	fi
done
