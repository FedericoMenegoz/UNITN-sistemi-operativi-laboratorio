#!/bin/bash

[[ "$1" = "-h" ]] && seq --help | head -n 4 && exit 0

if [[ $# -eq 1 ]]; then 
    i=1
    while [[ i -le $1 ]]; do
        echo $i
        (( i++ ))
    done
elif [[ $# -eq 2 ]]; then
    i=$1
    while [[ i -le $2 ]]; do
        echo $i
        (( i++ ))
    done
elif [[ $# -eq 3 ]]; then
    i=$1
    while [[ i -le $3 ]]; do
        echo $i
        i=$(( i + $2 ))
    done
else
    seq --help | head -n 4 && exit 1
fi