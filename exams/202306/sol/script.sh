#!/bin/bash
# Args check
[[ $# != 10 ]] && echo "Usage: ./script <word_1> ... <word_10>" && exit 1

list=("$@")

# method 1
sorted=($(printf '%s\n' "${list[@]}" | sort))
echo ${sorted[@]}


#method 2
# for word in ${list[@]};do
#     echo $word >> tmp.txt
# done

# cat tmp.txt | sort
# rm tmp.txt

