#!/bin/bash

bold=$(tput bold)

# start stop underline
underline=$(tput smul)
non_underline=$(tput rmul)

#set text colour
red=$(tput setaf 1)
# set background colour
back=$(tput setab 1)
# reset
reset=$(tput sgr0)

echo -e "This is normal.\n${bold}This is bold.\n${red}This is bold and red.\n${reset}${red}This is red.\n${reset}Back to normal."

echo "${italic}Back to normal.${reset} -Ciao"