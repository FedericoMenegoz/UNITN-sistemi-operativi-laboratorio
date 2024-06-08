#!/bin/bash
TMP=/tmp/tmp.txt
[ -e "$TMP" ] && rm "$TMP"
# Arg check
if [ $# -eq 2 ]; then
    # Check if it is a valid directory
    cd "$1" 2>/dev/null || { echo "$1 is not a valid directory"; exit 2; }
    for i in *; do 
        [ -f "$i" ] && wc -l "$i" >> "$TMP"
    done 
else 
    echo "Usage: ./lines_counter.sh <dir> [up|down]"
    exit 3
fi

if [ "$2" = "up" ]; then 
    sort -n < "$TMP" 
elif [ "$2" = "down" ]; then 
    sort -r -n < "$TMP"
else
    echo "ERROR: second argument must be 'up' or 'down'" && exit 4
fi
