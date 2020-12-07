# Temporal hybridization number

This repository contains an implementation of the algorithms for computing the temporal hybridization number from the 
master thesis _'New FPT algorithms for finding the temporal hybridization number for sets of phylogenetic trees'_.
Because the code has been optimized for speed, some functions are more complex than strictly necessary.

This code is meant as a proof of concept implementation to show that competetive running times can be reached using the FPT algorithm.
The implementation might contain bugs.

The code can be compiled on linux with:

```
cmake .  
make cherrypick_cpp
```

For more information on running the program run

```
./cherrypicking_cpp -h
```