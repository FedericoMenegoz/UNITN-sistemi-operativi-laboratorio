#!/bin/bash

# Define colors for output
bold=$(tput bold)
reset=$(tput sgr0)
CORRECT="${bold}CORRECT${reset}"
INCORRECT="${bold}INCORRECT${reset}"

# Function to run the test
run_test() {
    local fifo="$1"
    local input="$2"
    local correct_content="$3"
    local program="$4"
    local param="$5"
    local expected_return="$6"

    echo "${bold}TESTING $program...${reset}"

    # Write the input to the FIFO using the writer program
    ./test/writer "$fifo" "$input" &
    sleep 1

    # Capture the program's output
    program_output=$(./sol/"$program" "$fifo" "$param")
    actual_return=$?

    wait

    # Check the return code
    if [ "$actual_return" -eq "$expected_return" ]; then
        echo -e "Return $expected_return? \t$CORRECT"
    else
        echo -e "Return $expected_return? $?\t$INCORRECT"
    fi

    # Check the stdout by comparing directly with correct_content
    if [ "$program_output" == "$correct_content" ]; then
        echo -e "STDOUT\t\t$CORRECT"
    else
        echo -e "STDOUT\t\t$INCORRECT"
    fi
}

# Variables
FIFO="path_to_fifo"
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
run_test "$FIFO" "$INPUT" "$CORRECT_CONTENT" "fifoget" 9 0
run_test "$FIFO" "$INPUT" "$CORRECT_CONTENT" "fifoget" 10 1
