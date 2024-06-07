#!/bin/bash
# Check if args are provided
[[ ! $# -eq 4 ]] && echo "Usage: $0 <path> <pattern1> <pattern2> <pattern3>" 1>&2 && exit 1

# Is file valid? 
[ ! -f $1 ] && echo "ERROR? $1 not a valid file" 1>&2 && exit 1

# Loop through the arg starting from the second for 3 times
for pattern in ${@:2:3}; do
    # cat -n will print the content of the file and adding the line numbers
    # grep will print the lines in which occurs a match 
    # cut -f1 will print only the line numbers 
    VAR=$(echo $(cat -n $1 | grep $pattern | cut -f1))
    echo "$pattern: $VAR"
done