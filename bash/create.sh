#!/bin/bash

echo "$@" | grep -e "-h" 1>/dev/null && echo "This should be helping." && exit 0

i=1
while [[ $i -le $1 ]]; do
    mkdir "$i" 2>/dev/null || ( echo "ERROR? $i exist already" && exit 1 )
    j=1
    while [[ $j -le $2 ]]; do
        if [[ -f $3 ]]; then 
            cp $3 $i/$j
        else 
            touch $i/$j
        fi
        (( j++ ))
    done
    (( i++ ))
done
        