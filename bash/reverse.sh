#!/bin/bash

# Write a sheel script that takes 3 arguments and prints them in reverse order.
# If -h is entered anywhere a short description should be printed as well.

if [[ $# -eq 3 ]]; then
    # grep -e is needed as the pattern start with -
    echo "$@" | grep -e "-h" 1>/dev/null && echo -e "\nShort description\n"
    echo "$3 $2 $1"
else 
    echo "Usage: $0 with 3 args! -h for help..maybe" && exit 1
fi
