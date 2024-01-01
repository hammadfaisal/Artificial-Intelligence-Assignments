#!/bin/bash
g++ -o main main.cpp SportsLayout.cpp

if [ $# -ne 2 ]; then
    echo "Give two arguments: the input file and the output file."
    exit 1
fi

./main "$1" "$2"
