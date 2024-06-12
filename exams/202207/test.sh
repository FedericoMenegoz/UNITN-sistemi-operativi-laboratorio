#!/bin/bash
FIFO=./sol/fifo

bold=$(tput bold)
# start stop underline
underline=$(tput smul)
non_underline=$(tput rmul)
#set text colour
red=$(tput setaf 1)
green=$(tput setaf 2)
grey=$(tput setaf 0)
# set background colour
back=$(tput setab 1)
# reset
reset=$(tput sgr0)

CORRECT="${green}CORRECT${reset}"
INCORRECT="${red}INCORRECT${reset}"

# fifoget
INPUT="ABCDE1234"
cat > ./correct.txt << EOL
A
B
C
D
E
1
2
3
4
EOL
echo "${bold}TESTING FIFOGET...${reset}"

./test/writer "$FIFO" "$INPUT" &
sleep 1 && ./sol/fifoget "$FIFO" 9 1>tmp.txt && echo -e "Return 0? \t$CORRECT" || echo -e "Return 0? $?\t$INCORRECT"
wait 
diff --color tmp.txt correct.txt && echo -e "STDOUT\t\t$CORRECT" || echo -e "STDOUT\t\t$INCORRECT"

./test/writer "$FIFO" "$INPUT" &
sleep 1 && ./sol/fifoget "$FIFO" 10 1>tmp.txt || echo -e "Return 1? \t$CORRECT" || echo -e "Return 0? $?\t$INCORRECT"
wait 
diff --color tmp.txt correct.txt && echo -e "STDOUT\t\t$CORRECT" || echo -e "STDOUT\t\t$INCORRECT"

# fifoskp
INPUT="ABCDE12345"
n=5
cat > ./correct.txt << EOL
A
B
C
D
1
2
3
4
EOL
echo "${bold}TESTING FIFOSKP...${reset}"