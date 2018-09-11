#!/bin/bash
#Hodaya koslowsky
#task number3

if [ "$#" -ne 1 ]; then
  echo "‫‪error:‬‬ ‫‪only‬‬ ‫‪one‬‬ ‫‪argument‬‬ ‫‪is‬‬ ‫‪allowed‬‬"
  exit
fi

if [ ! -f $1 ]; then
  echo "‫‪error:‬‬ ‫‪there‬‬ ‫‪is‬‬ ‫‪no‬‬ ‫‪such‬‬ ‫‪file‬‬"
  exit
fi

if [ ! -d ./safe_rm_dir ]; then
  mkdir ./safe_rm_dir
fi

cp $1 ./safe_rm_dir
rm $1
echo "done!"
