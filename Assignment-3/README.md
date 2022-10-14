# Assignment 3

## Directory structure
```
├── code
│   ├── inputs
│   │   ├── problemsize
│   │   ├── testinput
│   │   └── timeinput
│   ├── Makefile
│   ├── validate.py
│   │      Scripts to validate the consistency of output wire routes and cost array.
│   │      Run "python validate.py -h" to see the instructions to use the script
│   ├── WireGrapher.java
│   │      Graph the input wires, see handout for instructions
│   ├── wireroute.cpp
│   │      The starter code for reading and parsing command line arguments is provided.
│   └── wireroute.h
├── examples: Examples for OpenMP
│   ├── hello.c
│   ├── loop.c
│   ├── Makefile
│   ├── README
│   └── sqrt.c
├── tutorials
│   ├── openmp.pdf
│   │       OpenMP tutorial
│   └── machines.pdf
│           PSC Bridges 2 tutorial
└── README.md
```

## Get started
0. Read handout.
1. Read README.
2. Read wireroute.cpp and wireroute.h including comments.
3. Define `wire_t` and any additional data structures you need.
4. Implement the serial version of the algorithm and test locally using `make`
   - You can use validate.py to validate the result.
5. Parallelize the algorithm and test locally.
6. Answer questions on the handout.