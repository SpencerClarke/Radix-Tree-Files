#!/bin/bash

if [ -z $1 ] || [ -z $2 ]
then
	echo "tokenize.sh: Pass in the .rdt and a japanese sentence as params 1 and 2"
	exit 4
else
	rdt=`realpath $1`
fi

cd "$(dirname "$0")"
sentence=$2
string_length=${#sentence}
start_index=0
while [ $start_index -lt $string_length ]
do
	substring_length=1
	hit_length=0
	while [ `expr $start_index + $substring_length` -le $string_length ]
	do
		substring=${sentence:$start_index:$substring_length}
		../bin/search $rdt $substring > /dev/null 2>&1
		ret=$?
		if [ $ret -eq 0 ]
		then
			hit_length=$substring_length
		fi
		
		substring_length=`expr $substring_length + 1`
	done
	if [ $hit_length -eq 0 ]
	then
		echo "Cannot tokenize" >&2
		exit 4
	fi
	
	echo ${sentence:$start_index:$hit_length}
	start_index=`expr $start_index + $hit_length`
done
