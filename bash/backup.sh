#!/bin/bash

if [ ! $# -eq 2 ]; then
    echo "Usage: ./backup.sh <name> <backup name>"
    exit 1
else 
    if [ -d "$1" ]; then
        len=$(( ${#1} - 1 ));
        [[ "${1:len:1}" == "/" ]] && bkpname=${1:0:len}_$2 || bkpname=$1_$2
        mkdir "$bkpname" && cp -r "$1" "$bkpname" && echo "Made backup of $1 into $bkpname."
    elif [ -f "$1" ]; then
        for i in {1..5}; do
            cp "$1" "$1*$i$2"
        done
        echo "Made 5 copies of $1."
    else 
        echo "$1 not a valid file or directory."
        exit 1
    fi
fi