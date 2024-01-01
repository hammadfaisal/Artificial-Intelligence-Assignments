#!/bin/bash
g++ ./solver.cpp -o solver
g++ ./mapper.cpp -o mapper
test=$1
./solver $test
minisat $test.satinput $test.satoutput
./mapper $test
