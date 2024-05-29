#!/bin/bash
if [ $# -eq 0 ]; then
    ps aux 
elif [ $# -eq 1 ] && [[ $1 =~ ^[0-9]+$ ]]; then
    ps aux | head -n $(( $1 + 1 ))
else
    echo "Usage: ./ps_monitor.sh <N>"
    echo "N must be a positive integer."
    exit 1
fi