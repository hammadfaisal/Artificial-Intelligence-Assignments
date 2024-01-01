# Quadratic Assignment Problem
There are a sets of zones and location such that number of zones is less than number of locations. The goal is to assign each zone to a location. There are matrices denoting number of daily walks between zones and time to travel between locations. The goal is to map zones to location to minimize the total time of walks.

## Algorithm
Greedy hill climbing with tabu list and random restarts is used to solve the problem.

## Usage
```
./run.sh <input_file> <output_file>
```
#### Input file format
```
<Availabe time>
<Number of zones>
<Number of locations>
<z x z matrix denoting no. of walks between zones>
<l x l matrix denoting time to travel between locations>
```
