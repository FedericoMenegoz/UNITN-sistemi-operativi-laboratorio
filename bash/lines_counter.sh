#!/bin/bash
TMP=/tmp/tmp.txt
[ -e $TMP ] && rm $TMP
# Arg check
if [ $# -eq 2 ]; then
    # Check if it is a valid directory
    if [ -d $1 ]; then
        cd $1
        for i in *; do 
            [ -f $i ] && wc -l $i >> $TMP
        done
    else 
        echo "$1 is not a valid directory" && exit 1
    fi
else 
    echo "Usage: ./lines_counter.sh <dir> [up|down]"
    exit 1
fi

if [ $2 = "up" ]; then 
    cat $TMP | sort -n
elif [ $2 = "down" ]; then 
    cat $TMP | sort -r -n
else
    echo "ERROR: second argument must be 'up' or 'down'"
fi
