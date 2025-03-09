# psuedorandom-sequences

CP431 Assignment 2: Psuedorandom-Sequences

# Running locally

Installing mpich (WSL if on Windows):

```
sudo apt install mpich
```

Compiling:

```
mpicc -o pseudorandom.exe main.c -lm -lmpi
```

Running:

```
mpirun -np <number-of-cores> ./pseudorandom.exe <N>
```

Example for N=10 with 8 cores:

```
mpirun -np 8 ./pseudorandom 10
```
