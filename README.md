# Pseudorandom Sequences

## Description
This project implements an MPI-based parallel program in C to search for pseudorandom sequences as defined by Donald Knuth in *The Art of Computer Programming, Volume 2*. The program systematically tests all sequences of length *N* from the boolean cube \{-1, +1\}^N to determine which satisfy the pseudorandomness condition.

## Requirements
- The program must be executed on a SHARCNET cluster.
- The implementation follows MPI (Message Passing Interface) standards for parallel computation.
- The program verifies pseudorandom sequences and searches for all valid sequences for lengths **20 ≤ N ≤ 30**.

## Installation
### Installing MPI (for local testing on WSL/Linux):
```bash
sudo apt install mpich
```

## Compilation
To compile the program, run:
```bash
mpicc -o pseudorandom.exe main.c -lm -lmpi
```

## Running the Program
To execute the program with a specified sequence length *N* using multiple cores:
```bash
mpirun -np <number-of-cores> ./pseudorandom.exe <N>
```
### Example
For *N=10* using *8 cores*:
```bash
mpirun -np 8 ./pseudorandom.exe 10
```

## Output
- The program finds all pseudorandom sequences for *N* in the range **20 ≤ N ≤ 30**.
- Results are stored in **11 separate files** in the `/scratch` directory, named:
  - `pr.20.txt`
  - `pr.21.txt`
  - ...
  - `pr.30.txt`
- The final report must include the source code and a table listing the number of pseudorandom sequences found for each *N*.

## Implementation Details
1. A C function checks if a given sequence satisfies the pseudorandomness condition.
2. The MPI program distributes tasks efficiently to test all possible sequences.
3. The solution leverages parallelism to optimize performance.
4. The results are stored for further analysis.
