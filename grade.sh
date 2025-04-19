#!/bin/bash
set -eu

srcfn="sh.c"
total=8
ecnt=0

RESET='\033[0m'
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'

print_result() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}PASS${RESET}"
    else
        echo -e "${RED}FAIL${RESET}"
    fi
}

# Test 1: Simple ls command
echo -e "${BLUE}Running Test 1: Simple ls command...${RESET}"
if ! tests/test1.sh $srcfn ; then
    ecnt=$(( $ecnt + 1 ))
    print_result 1
else
    print_result 0
fi

# Test 2: Sequence of simple ls commands
echo -e "${BLUE}Running Test 2: Sequence of simple ls commands...${RESET}"
if ! tests/test2.sh $srcfn ; then
    ecnt=$(( $ecnt + 1 ))
    print_result 1
else
    print_result 0
fi

# Test 3: Output redirection command
echo -e "${BLUE}Running Test 3: Output redirection command...${RESET}"
if ! tests/test3.sh $srcfn ; then
    ecnt=$(( $ecnt + 1 ))
    print_result 1
else
    print_result 0
fi

# Test 4: Input redirection command
echo -e "${BLUE}Running Test 4: Input redirection command...${RESET}"
if ! tests/test4.sh $srcfn ; then
    ecnt=$(( $ecnt + 1 ))
    print_result 1
else
    print_result 0
fi

# Test 5: Input/output redirection command
echo -e "${BLUE}Running Test 5: Input/output redirection command...${RESET}"
if ! tests/test5.sh $srcfn ; then
    ecnt=$(( $ecnt + 1 ))
    print_result 1
else
    print_result 0
fi

# Test 6: Pipe command
echo -e "${BLUE}Running Test 6: Pipe command...${RESET}"
if ! tests/test6.sh $srcfn ; then
    ecnt=$(( $ecnt + 1 ))
    print_result 1
else
    print_result 0
fi

# Test 7: Sequence of pipe commands
echo -e "${BLUE}Running Test 7: Sequence of pipe commands...${RESET}"
if ! tests/test7.sh $srcfn ; then
    ecnt=$(( $ecnt + 1 ))
    print_result 1
else
    print_result 0
fi

# Test 8: Sequence of pipe commands with output redirection
echo -e "${BLUE}Running Test 8: Sequence of pipe commands with output redirection...${RESET}"
if ! tests/test8.sh $srcfn ; then
    ecnt=$(( $ecnt + 1 ))
    print_result 1
else
    print_result 0
fi

# Summary
echo -e "${YELLOW}Test Summary:${RESET}"
echo -e "${GREEN}Your code passed $(( $total - $ecnt )) of $total tests.${RESET}"

exit 0
