#!/bin/bash

if [ -z $1 ] || [ -z $2 ]
then
	echo "quiz.sh: Pass in the .rdt and a file containing 1 japanese word per line as params 1 and 2"
	exit 4
else
	rdt=`realpath $1`
	word_list=`realpath $2`
fi

cd "$(dirname "$0")"

for word in $(cat $word_list)
do
	echo $word
	read user_input
	dictionary_output=$(../bin/search $rdt $word)
	correct=false
	for token in $dictionary_output
	do
		if [ $user_input = $token ]
		then
			echo "Correct"
			correct=true
			break
		fi
	done

	if [ $correct = false ]
	then
		echo "Incorrect"
		echo $dictionary_output
	fi
done
