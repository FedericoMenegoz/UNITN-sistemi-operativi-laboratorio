#!/bin/bash
i=0
while read -r line; do
    [ $(( $i % 2 )) -eq 0 ] && echo $line || echo $line 1>&2
    (( i++ ))
done < "$1"