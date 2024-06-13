#!/bin/bash

# Define colors for output
bold=$(tput bold)
grey=$(tput setaf 0)
red=$(tput setaf 1)
green=$(tput setaf 2)
blue=$(tput setaf 4)
reset=$(tput sgr0)
underline=$(tput smul)
non_underline=$(tput rmul)
CORRECT="${green}${bold}CORRECT${reset}"
INCORRECT="${red}${bold}INCORRECT${reset}"

# Function to create and manage FIFO
create_fifo() {
    local fifo="$1"
    if [ -p "$fifo" ]; then
        rm "$fifo"
    fi
    mkfifo "$fifo"
}

# Function to run the test
run_test() {
    local input="$1"
    local correct_content="$2"
    local program="$3"
    local n="$4"
    local expected_return="$5"
    local fifo="./sol/fifo"

    echo "${bold}TESTING ${underline}${blue}$program...${reset}"

    # Create FIFO
    create_fifo "$fifo"

    # Write the input to the FIFO using the writer program
    ./test/writer "$fifo" "$input" &
    # sleep 1

    # Capture the program's output
    program_output=$("$program" "$fifo" "$n")
    actual_return=$?


    # Check the return code
    if [ "$actual_return" -eq "$expected_return" ]; then
        echo -e "${grey}Return $expected_return? \t$CORRECT"
    else
        echo -e "${grey}Return $expected_return? $?\t$INCORRECT"
    fi

    # Check the stdout by comparing directly with correct_content
    if [ "$program_output" == "$correct_content" ]; then
        echo -e "${grey}STDOUT\t\t$CORRECT"
    else
        echo -e "STDOUT\t\t$INCORRECT"
        echo "${bold}${red}Program output:${reset}"
        echo -e "${grey}$program_output${reset}"
        echo "${bold}${green}Correct output:${reset}"
        echo -e "${grey}$correct_content${reset}"
    fi
    # Remove FIFO
    rm "$fifo"
    echo ""
    sleep 1
}

# Check if binaries exist
BINARIES=("fifoget" "fifoskp" "fiforev" "fifoply")
for bin in "${BINARIES[@]}"; do
    if [ ! -e ./sol/$bin ]; then
        echo -e "${bold}ERROR?\t${reset}${red}./sol/$bin not found${reset}" && exit 1
    fi
done

# FIFOGET
INPUT="ABCDE1234"
CORRECT_CONTENT=$(cat <<EOL
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
)

# Test cases
run_test "$INPUT" "$CORRECT_CONTENT" "./sol/fifoget" 9 0
run_test "$INPUT" "$CORRECT_CONTENT" "./sol/fifoget" 10 1

# FIFOSKP
INPUT="ABCDE1234"
CORRECT_CONTENT=$(cat <<EOL
A
B
C
D
E
1
2
3
EOL
)

# Test cases
run_test "$INPUT" "$CORRECT_CONTENT" "./sol/fifoskp" 9 0
CORRECT_CONTENT=$(cat <<EOL
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
)
run_test "$INPUT" "$CORRECT_CONTENT" "./sol/fifoskp" 10 1

# FIFOREV
INPUT="4321EDCBA"
CORRECT_CONTENT=$(cat <<EOL
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
)

# Test cases
run_test "$INPUT" "$CORRECT_CONTENT" "./sol/fiforev" 9 0
run_test "$INPUT" "$CORRECT_CONTENT" "./sol/fiforev" 10 1

# FIFOPLY
INPUT="RFS123A"
CORRECT_CONTENT=$(cat <<EOL
A
S
F
R
3+2+1=6
EOL
)

# Test cases
run_test "$INPUT" "$CORRECT_CONTENT" "./sol/fifoply" 7 0
run_test "$INPUT" "$CORRECT_CONTENT" "./sol/fifoply" 10 1
