#!/bin/bash
#Hodaya koslowsky
#task number4

filename=$2
total=0
while read line; do 

  arr=($line)
  name="${arr[@]:0:2}"

  if [ "$name" == "$1" ]
  then
    echo $line
    num=${arr[2]}
    total=$(($total + $num))
  fi
done < $filename
echo "Total balance:" $total
